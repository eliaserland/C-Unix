/*
 * mexec - Execute an arbitrary pipeline of commands, specified by file or stdin. 
 *
 * OU1 for C programming and Unix, Umea University Autumn 2021.
 * 
 * Author: Elias Olofsson (tfy17eon@cs.umu.se) 
 *
 * Version information:
 *   2021-09-21: v1.0, first public version.   
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1
#define BUFSIZE 1024

int  open_pipes			(int n, int ***pipeID);
int  spawn_children		(int proc_cnt, int *child_no);
int  dup_pipe			(int proc_cnt, int child_no, int **pipeID);
void close_pipes		(int pipe_cnt, int **pipeID);
int  exec_cmd			(char **args);
int  wait_on_children	(int proc_cnt);
void kill_all			(int proc_cnt, int **pipeID, char ***prog_cmds);

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
	
	int arg_cnt, proc_cnt = 0;
	char buffer[BUFSIZE], *token, **args;
	char ***prog_cmds = NULL;  
	
	/* Read from the specified stream, one line at a time, until EOF. */
	while (fgets(buffer, BUFSIZE, input) != NULL) {
		buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline character.
		arg_cnt = 0;
		args = NULL;
		
		/* Split buffer string into arguments, delimited by whitespace. */
		token = strtok(buffer, " ");
		while (token != NULL) {
			args = realloc(args, (arg_cnt+1)*sizeof(char*));
			if (args == NULL) {
				perror("realloc error");
				exit(EXIT_FAILURE);
			}
			if ((args[arg_cnt++] = strdup(token)) == NULL) {
				perror("strdup error");
				exit(EXIT_FAILURE);
			}
			token = strtok(NULL, " ");
		}
		
		/* Add extra NULL required by exec. */
		args = realloc(args, (arg_cnt+1)*sizeof(char*));
		if (args == NULL) {
			perror("realloc error");
			exit(EXIT_FAILURE);
		}
		args[arg_cnt] = NULL; 
		
		/* Save the parsed command and arguments. */        
		prog_cmds = realloc(prog_cmds, (proc_cnt+1)*sizeof(char**));
		if (prog_cmds == NULL) {
			perror("realloc error");
			exit(EXIT_FAILURE);
		}
		prog_cmds[proc_cnt++] = args;
	}
	if (argc == 2) {
		fclose(input); // Only close stream if reading from file.
	}
	
	
	/* Open the (proc_cnt-1) pipes required. */
	int **pipeID;
	if (proc_cnt > 1 && open_pipes(proc_cnt-1, &pipeID) != 0) {
		// kill prog_cmds	
		exit(EXIT_FAILURE);
	}
		
	// -------- Can use kill_all() from here on ------------
	
	/* Create the (proc_cnt) children processes */
	int child_no;
	if (spawn_children(proc_cnt, &child_no) != 0) {
		kill_all(proc_cnt, pipeID, prog_cmds);
		exit(EXIT_FAILURE);
	}
	
	/* Duplicate pipe file descriptors to stdin/stdout appropriately for all children. */
	if (child_no > -1 && dup_pipe(proc_cnt, child_no, pipeID) != 0) {
		kill_all(proc_cnt, pipeID, prog_cmds);
		exit(EXIT_FAILURE);
	}  
	
	/* Close all pipes (parent & children). */
	close_pipes(proc_cnt, pipeID);
	   
	/* Exec on all children. */
	if (child_no > -1 && exec_cmd(prog_cmds[child_no]) != 0) {
		kill_all(proc_cnt, pipeID, prog_cmds);
		exit(EXIT_FAILURE);
	} 
	
	/* Let parent wait on all children. */	
	if (wait_on_children(proc_cnt) != 0) {
		kill_all(proc_cnt, pipeID, prog_cmds);
		exit(EXIT_FAILURE);
	}


	/* Free dynamically allocated memory */
	kill_all(proc_cnt, pipeID, prog_cmds);
		
	return 0;
}


/**
 * Open a specified number of pipes.
 *
 * @param n			The number of pipes to create.
 * @param pipeID	Pointer to the created 2D array of pipe file descriptors. 
 * @return			0 upon successful creation of all pipes.
 */
int open_pipes(int n, int ***pipeID) 
{
	fprintf(stderr, "Hello1\n");
	*pipeID = malloc(n * sizeof(int*));
	if (*pipeID == NULL) {
		perror("malloc");
		fprintf(stderr, "Hello2\n");
		goto err0;
	}
	fprintf(stderr, "Hello3\n");
	int i;
	for (i = 0; i < n; i++) {
		*pipeID[i] = malloc(2 * sizeof(int));
		if (*pipeID[i] == NULL) {
			perror("malloc");
			fprintf(stderr, "Hello4\n");
			goto err1;
		}
		fprintf(stderr, "Hello5\n");
		if (pipe(*pipeID[i]) != 0) {
			fprintf(stderr, "Hello6\n");
			perror("pipe");
			goto err2;
		}
		fprintf(stderr, "Hello7\n");
	}
	return 0;

err2:
	free(*pipeID[i]);
err1:
	for (int j = i-1; j >= 0; j--) {
		free(*pipeID[j]);
	}
	free(*pipeID);
err0: 
	return 1;
}

/**
 * Create a specified number of chilren processes.
 *
 * @param proc_cnt	The number of children to create.
 * @param child_no	Used to set a unique identifer of the children, gives the 
 *					relative order among the processes. Parent is set to -1.
 * @return			0 upon successful creation of all children.
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
 * For a given child process, duplicate pipe file descriptors to stdin and/or stdout.
 *
 * @param proc_cnt  The total number of children processes.
 * @param child_no  Unique identifier of the child, giving the relative order.
 * @param pipeID    2D array of pipe file descriptors.
 * @return			0 opon successful creation of all pipes.
 */
int dup_pipe(int proc_cnt, int child_no, int **pipeID) 
{
	if (child_no == 0) {
		// First child	
		if (dup2(pipeID[child_no][WRITE_END], STDOUT_FILENO) == -1) {
			goto err;
		}
	} else if (child_no == proc_cnt-1) {
		// Last child
		if (dup2(pipeID[child_no-1][READ_END], STDIN_FILENO) == -1) {
			goto err;
		}
	} else {
		// Intermediate children
		if (dup2(pipeID[child_no-1][READ_END], STDIN_FILENO) == -1) {
			goto err;
		}
		if (dup2(pipeID[child_no][WRITE_END], STDOUT_FILENO) == -1) {
			goto err;
		}
	}
	return 0;
err:
	perror("dup2");
	return 1;
}	

/**
 * Close all pipes.
 *
 * @param pipe_cnt  The number of open pipes.
 * @param pipeID    2D array of pipe file descriptors.
 */
void close_pipes(int pipe_cnt, int **pipeID) 
{
	for (int i = 0; i < pipe_cnt-1; i++) {
		for (int j = 0; j < 2; j++) {
			close(pipeID[i][j]);
		}
	}
}

/**
 * Execute a program with arguments.
 *
 * @param args	Array of strings, representing program command with arguments.
 * @return 		Returns 1 if exec fails.
 */
int exec_cmd(char **args) 
{
	if (execvp(args[0], args) < 0) {
		perror(args[0]);
		return 1;
	}
	return 0; // This line is never executed, but the compiler complains if I remove it. 	
}

/**
 * Let the process (assumed to be parent) wait on all its children.
 *
 * @param proc_cnt  The number of children processes.
 * @return 			Returns 0 if successfully waited on all children.
 */
int wait_on_children(int proc_cnt) 
{
	int status;
 	for (int i = 0; i < proc_cnt; i++) {
 		if ((wait(&status)) == -1) {
 			perror("wait");
 			return 1;
 		}
 		if (WEXITSTATUS(status) != 0) {
 			fprintf(stderr, "Child process exited with error.\n");
			return 1;
 		}
 	}
 	return 0;
}

/**
 * Return all dynamically allocated memory.
 *
 * @param proc_cnt  The number of chilren processes/program commands.
 * @param pipeID    2D array of pipe file descriptors.
 * @param prog_cmds 2D array of program commands.
 */
void kill_all(int proc_cnt, int **pipeID, char ***prog_cmds) 
{
	/* Freeing dynamically allocated memory */ 
	for (int i = 0; i < proc_cnt; i++) {
		int j = 0; 
		while (prog_cmds[i][j] != NULL) {
			free(prog_cmds[i][j]);
			j++;
		}
		free(prog_cmds[i]);
	}
	free(prog_cmds);
	for (int i = 0; i < proc_cnt-1; i++) {
		free(pipeID[i]);
	}
	if (proc_cnt > 1) {
		free(pipeID);
	}
}

