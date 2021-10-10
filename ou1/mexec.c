/*
 * mexec - Execute an arbitrary pipeline of commands, specified by file or stdin.
 *
 * OU1 for C programming and Unix, Umea University Autumn 2021.
 *
 * Author: Elias Olofsson (tfy17eon@cs.umu.se)
 *
 * Version information:
 *   2021-09-21: v1.0, first public version.
 *   2021-10-10: v2.0. Split the program up into individual functions, such that
 *        not all is contained in the main function. Improved error handling
 *        such that no memory leak occur if problems are encountered at runtime.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "list.h"

#define READ_END 0
#define WRITE_END 1
#define BUFSIZE 1024

int   save_token      (char **cmd[], char *token, int count);
list *parse_commands  (FILE *input);
int   spawn_children  (int proc_cnt, int *child_no);
int   dup_pipe        (int proc_cnt, int child_no, int **pipeID);
int   open_pipe       (int *p[]);
int   open_pipes      (int n, int ***pipeID);
void  close_pipes     (int n, int **pipeID);
int   exec_cmd        (char *argv[]);
int   safe_wait       (void);
int   wait_on_children(int n);
void  kill_all        (int **pipeID, list *prog_cmds);
void  free_cmd        (void *ptr);

int main (int argc, char *argv[])
{
	FILE *input;
	if (argc == 1) {
		// Read from stdin
		input = stdin;
	} else if (argc == 2) {
		// Read from file
		if ((input = fopen(argv[1], "r")) == NULL) {
			perror(argv[1]);
			exit(EXIT_FAILURE);
		}
	} else {
		// Too many arguments.
		fprintf(stderr, "Usage: ./mexec [FILE]\n");
		exit(EXIT_FAILURE);
	}

	int process_count, child_no, pipe_count = 0;
	int **pipeID = NULL;
	char **cmd;
	list *prog_cmds;

	do {
		prog_cmds = parse_commands(input);
		if (argc == 2) {
			fclose(input); // Only close stream if reading from file.
		}
		if (prog_cmds == NULL) {
			break;
		}

		/* No. of processes and pipes required. */
		process_count = list_length(prog_cmds);
		pipe_count = process_count-1;

		/* Open pipes, if required. */
		if (pipe_count && open_pipes(pipe_count, &pipeID)) {
			break;
		}

		/* Create the children processes */
		if (spawn_children(process_count, &child_no)) {
			break;
		}

		/* Duplicate pipe file descriptors to stdin/stdout for all children. */
		if (pipe_count && child_no > -1) {
			if (dup_pipe(process_count, child_no, pipeID)) {
				break;
			}
		}

		/* Close all pipes (parent & children). */
		close_pipes(pipe_count, pipeID);

		/* Execute program command in all children. */
		if (child_no > -1) {
			cmd = list_inspect(prog_cmds, list_index(prog_cmds, child_no));
			if (exec_cmd(cmd)) {
				break;
			}
		}

		/* Let parent wait on all children. */
		if (wait_on_children(process_count)) {
			break;
		}

		/* Free dynamically allocated memory */
		kill_all(pipeID, prog_cmds);

		/* Normal exit. */
		return 0;

	} while (0);

	/* Error handling. */
	close_pipes(pipe_count, pipeID);
	kill_all(pipeID, prog_cmds);
	exit(EXIT_FAILURE);
}

/**
 * save_token() - Copy and save a string to a pre-existing array of strings.
 * @cmd:    Pointer to the array of strings.
 * @token:  Null-terminated string to be saved to the array, or NULL.
 * @count:  Current length of the array, excluding the token to be added.
 *
 * Returns: 0 on success.
 */
int save_token(char **cmd[], char *token, int count)
{
	// Resize the array.
	char **tmp = realloc(*cmd, (count+1)*sizeof(char*));
	if (tmp == NULL) {
		perror("realloc");
		return 1;
	}
	*cmd = tmp;
	// Add the string, or NULL.
	if (token == NULL) {
		(*cmd)[count] = NULL;
	} else {
		(*cmd)[count] = strdup(token);
		if ((*cmd)[count] == NULL) {
			perror("strdup");
			return 1;
		}
	}
	return 0;
}

/**
 * parse_commands() - Parse program commands from a given input file stream.
 * @input: The specified file input stream.
 *
 * Returns:	A list structure containing parsed program commands. Each list
 *          element points to an array of null-terminated strings. Returns NULL
 *          on error.
 */
list *parse_commands(FILE *input)
{
	int arg_cnt;
	char buffer[BUFSIZE], *token;
	char **cmd;

	list *l = list_empty(free_cmd);
	if (l == NULL) {
		return NULL;
	}

	// Read from the specified stream, one line at a time, until EOF.
	while (fgets(buffer, BUFSIZE, input) != NULL) {

		cmd = NULL;
		arg_cnt = 0;
		buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline character.

		// Split buffer string into arguments, delimited by whitespace.
		token = strtok(buffer, " ");
		while (token != NULL) {
			if (save_token(&cmd, token, arg_cnt)) {
				return NULL;
			}
			arg_cnt++;
			token = strtok(NULL, " ");
		}

		// Add extra NULL required by exec.
		if (save_token(&cmd, NULL, arg_cnt)) {
			return NULL;
		}

		// Save the parsed command and arguments.
		if (list_append(l, cmd)) {
			return NULL;
		}
	}
	return l;
}

/**
 * spawn_children() - Create a specified number of chilren processes.
 * @proc_cnt: The number of children to create.
 * @child_no: Used to set a unique identifer of the children, gives the
 *            relative order among the processes. Parent is set to -1.
 *
 * Returns:	  0 upon successful creation of all children.
 */
int spawn_children(int proc_cnt, int *child_no)
{
	pid_t pid;
	*child_no = -1;
	for (int i = 0; i < proc_cnt; i++) {
		pid = fork();
		if (pid < 0) {
			perror("fork");
			return 1;
		} else if (pid == 0) { // Only children
			*child_no = i;
			break;
		}
	}
	return 0;
}

/**
 * dup_pipe() - For a given child process, duplicate the appropriate pipe file
 *              descriptors to stdin and/or stdout.
 * @proc_cnt: The total number of children processes.
 * @child_no: Unique identifier of the child, giving the relative order.
 * @pipeID:   2D array of pipe file descriptors.
 *
 * Returns:   0 opon successful duplication.
 */
int dup_pipe(int proc_cnt, int child_no, int **pipeID)
{
	do {
		if (child_no == 0) {
			// First child
			if (dup2(pipeID[child_no][WRITE_END], STDOUT_FILENO) == -1) {
				break;
			}
		} else if (child_no == proc_cnt-1) {
			// Last child
			if (dup2(pipeID[child_no-1][READ_END], STDIN_FILENO) == -1) {
				break;
			}
		} else {
			// Intermediate children
			if (dup2(pipeID[child_no-1][READ_END], STDIN_FILENO) == -1) {
				break;
			}
			if (dup2(pipeID[child_no][WRITE_END], STDOUT_FILENO) == -1) {
				break;
			}
		}
		return 0;
	} while (0);
	// Error handling.
	perror("dup2");
	return 1;
}

/**
 * open_pipe() - Dynamically allocate memory and open a pipe.
 * @p: Pointer to the created pipe file descriptors.
 *
 * Returns: 0 upon successful creation of the pipe.
 */
int open_pipe(int *p[])
{
	*p = malloc(2 * sizeof(int));
	if (*p == NULL) {
		perror("malloc");
		return 1;
	}
	if (pipe(*p) != 0) {
		perror("pipe");
		return 1;
	}
	return 0;
}

/**
 * open_pipes() - Open a specified number of pipes.
 * @n:      The number of pipes to create.
 * @pipeID:	Pointer to the created 2D array of pipe file descriptors.
 *
 * Returns: 0 upon successful creation of all pipes.
 */
int open_pipes(int n, int ***pipeID)
{
	*pipeID = malloc(n * sizeof(int*));
	if (*pipeID == NULL) {
		perror("malloc");
		return 1;
	}
	for (int i = 0; i < n; i++) {
		(*pipeID)[i] = NULL;
	}
	for (int i = 0; i < n; i++) {
		if (open_pipe(&(*pipeID)[i])) {
			return 1;
		}
	}
	return 0;
}

/**
 * close_pipes() - Close all pipes in an array of pipe file descriptors.
 * @n:      The number of pipes in the array.
 * @pipeID: Array of pointers to pipe file descriptors.
 *
 * Returns: Nothing.
 */
void close_pipes(int n, int **pipeID)
{
	if (pipeID) {
		for (int i = 0; i < n; i++) {
			if (pipeID[i]) {
				close(pipeID[i][READ_END]);
				close(pipeID[i][WRITE_END]);
			}
		}
	}
}

/**
 * exec_cmd() - Execute a program with arguments.
 * @argv: Array of pointers to null-terminated strings, representing
 *        program arguments.
 *
 * Returns: 1 if exec fails.
 */
int exec_cmd(char *argv[])
{
	if (execvp(argv[0], argv) < 0) {
		perror(argv[0]);
		return 1;
	}
	return 0; // This is never executed, but the compiler complains if removed.
}

/**
 * safe_wait() - Wait on a child of the calling process. Similar to wait(),
 *               but checks errors.
 *
 * Returns: 0 on success, 1 on wait error or if child exited with errors.
 */
int safe_wait(void)
{
	int status;
	if (wait(&status) == -1){
		perror("wait");
 		return 1;
	}
	if (WEXITSTATUS(status) != 0) {
		return 1;
 	}
	return 0;
}

/**
 * wait_on_children() - Let the process (assumed to be parent) wait on all its
 *                      children processes.
 * @n: The number of children processes to wait on.
 *
 * Returns: 0 if successfully waited on all children.
 */
int wait_on_children(int n)
{
 	for (int i = 0; i < n; i++) {
 		if (safe_wait()) {
			return 1;
		}
 	}
 	return 0;
}

/**
 * kill_all() - Return all dynamically allocated memory.
 * @proc_cnt:  The number of chilren processes/program commands.
 * @pipeID:    2D array of pipe file descriptors.
 * @prog_cmds: 2D array of program commands.
 *
 * Returns: Nothing.
 */
void kill_all(int **pipeID, list *prog_cmds)
{
	// Kill pipe file descriptor array. NOTE: Pipes must first have been closed.
	int process_count;
	if (pipeID) {
		process_count = list_length(prog_cmds);
		for (int i = 0; i < process_count-1; i++) {
			free(pipeID[i]);
		}
	}
	free(pipeID);

	// Kill list of program commands.
	list_kill(prog_cmds);
}

/**
 * free_cmd() - Custom freeing function to deallocate memory used by the program
 *              commands. Should be passed as argument when the list structure
 *              to contain the program commands is created.
 * @ptr: Pointer to array of strings to be freed.
 *
 * Returns: Nothing.
 */
void free_cmd(void *ptr)
{
	char **cmd = ptr;
	for (int i = 0; cmd[i] != NULL; i++) {
		free(cmd[i]);
	}
	free(cmd);
}
