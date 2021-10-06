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
#include <stdbool.h>

#define READ_END 0
#define WRITE_END 1
#define BUFSIZE 1024


//typedef char **command;
//typedef command *command_array;


typedef struct command {
	struct command *next; // Pointer to next program command.
	char **argv;         // Array of pointers to null-terminated strings. Last item must be NULL.
} command;

typedef struct list {
	struct command *head;  // Pointer to the list head.
	int prog_cnt;          // The total number of program commands.
} list;


int safe_wait(void);


void *safe_malloc(size_t size);

int safe_strdup(const char *s, char **s2);

int save_token(char **cmd[], char *token, int count);

//int parse_commands(FILE *input, command_array *prog_cmds, int *prog_cnt);
//int parse_commands(FILE *input, list *l);
list *parse_commands(FILE *input);

int open_pipe(int *p[]);
int open_pipes(int n, int ***pipeID);
int  spawn_children		(int proc_cnt, int *child_no);
int  dup_pipe			(int proc_cnt, int child_no, int **pipeID);
void close_pipes		(int pipe_cnt, int **pipeID);

int exec_cmd(char *argv[]);

int  wait_on_children	(int proc_cnt);
void kill_all(int proc_cnt, int **pipeID, list *prog_cmds); 



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
	
	
	
	list *prog_cmds = parse_commands(input);
	if (prog_cmds == NULL) {
		// do something
		exit(EXIT_FAILURE);
	}
	int proc_cnt = prog_cmds->prog_cnt;
	
	/*
	if (parse_commands(input, &prog_cmds)) {
		// do something
		exit(EXIT_FAILURE);
	}
	*/
	if (argc == 2) {
		fclose(input); // Only close stream if reading from file.
	}
	
	fprintf(stderr, "BOOYA!\n");
	exit(EXIT_SUCCESS);
	
	
	bool pipes;
	if (proc_cnt > 1) {
		pipes = true;	
	} else {
		pipes = false;
	}
	
	
	
	/* Open the (proc_cnt-1) pipes required. */
	int **pipeID = NULL;
	
	if (pipes && open_pipes(proc_cnt-1, &pipeID)) {
		close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, prog_cmds);	
		exit(EXIT_FAILURE);
	}
	


	// -------- Can use kill_all() from here on ------------
	
	/* Create the (proc_cnt) children processes */
	int child_no;
	if (spawn_children(proc_cnt, &child_no) != 0) {
		close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, prog_cmds);
		exit(EXIT_FAILURE);
	}
	
	/* Duplicate pipe file descriptors to stdin/stdout appropriately for all children. */
	if (pipes && child_no > -1 && dup_pipe(proc_cnt, child_no, pipeID) != 0) {
		close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, prog_cmds);
		exit(EXIT_FAILURE);
	}  
	
	/* Close all pipes (parent & children). */
	close_pipes(proc_cnt-1, pipeID);
	   
	/* Exec on all children. */ 
	if (child_no > -1 && exec_cmd(prog_cmds[child_no]) != 0) { // EXEC NEEDS TO BE RE-DONE 
		//close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, prog_cmds);
		exit(EXIT_FAILURE);
	} 
	
	/* Let parent wait on all children. */	
	if (wait_on_children(proc_cnt)) {
		//close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, prog_cmds);
		exit(EXIT_FAILURE);
	}

	//close_pipes(proc_cnt-1, pipeID);

	/* Free dynamically allocated memory */
	kill_all(proc_cnt, pipeID, prog_cmds);
	
		
	return 0;
}





/** NOT UP TO DATE REDOOOOOOOO!!!
 * Duplicate a string. Same as strdup(), but exits on error.
 *
 * @param file		String containing the filename.
 * @return			true if file exists.
 */
int safe_strdup(const char *s, char **s2)
{
	if ((*s2 = strdup(s)) == NULL) {
		perror("strdup");
		return 1;
	}
	return 0;
}



/**
 * Copy and save a string to a command of a certain length. 
 *
 * @param cmd   The target command.
 * @param token Null-terminated string to be saved to the command.
 * @count count Current length of the command, excluding the token.
 * @return      0 on success.
 */
int save_token(char **cmd[], char *token, int count) 
{
	char **tmp = realloc(*cmd, (count+1)*sizeof(char*));
	if (tmp == NULL) {
		perror("realloc error");
		return 1;
	}
	*cmd = tmp;
	(*cmd)[count] = NULL; // REMOVE THIS ???
	if (token == NULL) {
		(*cmd)[count] = NULL;
	} else {
		(*cmd)[count] = strdup(token);	
		if ((*cmd)[count] == NULL) {
			perror("strdup error");
			return 1;
		}
	}		
	return 0;
}


/*
int parse_commands(FILE *input, list *l) 
{
	
	int *val = malloc(sizeof(int));
	*val = 5;
	
	list_insert(l, val, list_end(l));
	
	return 0;
}

*/


list *parse_commands(FILE *input) 
{
	int arg_cnt; 
	char buffer[BUFSIZE], *token;
	char **cmd;
	
	command *current;
	// int p_cnt = 0;
	// *prog_cmds = NULL;
	
	list *l = calloc(1, sizeof(list));
	if (l == NULL) {
		perror("calloc");
		return NULL;	
	}
	l->prog_cnt = 0;
	
	l->head = calloc(1, sizeof(command));
	if (l->head == NULL) {
		perror("calloc");
		return NULL;
	}
	current = l->head;
	
	// Read from the specified stream, one line at a time, until EOF.
	while (fgets(buffer, BUFSIZE, input) != NULL) {
		
		arg_cnt = 0;
		cmd = NULL;
		buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline character.
		
		// Split buffer string into arguments, delimited by whitespace. 
		token = strtok(buffer, " ");
		while (token != NULL) {
			if (save_token(&cmd, token, arg_cnt)) {
				// DO SOMETHING ELSE; KILL ALL
				return NULL; 
			}
			arg_cnt++;
			token = strtok(NULL, " ");
		}
		
		// Add extra NULL required by exec. 
		if (save_token(&cmd, NULL, arg_cnt)) {
			// DO SOMETHING ELSE, KILL ALL
			return NULL;
		}

		// Save the parsed command and arguments.      
		
		current->next = calloc(1, sizeof(command));
		if (current->next == NULL) {
			perror("calloc");
			// DO SOMETHING ELSE, KILL ALL
			return NULL;
		}
		current->next->argv = cmd;
		(l->prog_cnt)++;
		current = current->next;
	}
	return l;
}




/*

int arg_cnt, proc_cnt = 0;
	char buffer[BUFSIZE], *token = NULL;
	
	command cmd, *progcmds = NULL;
	 
	// Read from the specified stream, one line at a time, until EOF. 
	while (fgets(buffer, BUFSIZE, input) != NULL) {
		
		//--------------------------------------------------
		buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline character.
		arg_cnt = 0;
		cmd = NULL;
		
		// Split buffer string into arguments, delimited by whitespace.
		token = strtok(buffer, " ");
		while (token != NULL) {
			if (save_token(&cmd, token, arg_cnt)) {
				// do something
				exit(EXIT_FAILURE);
			}
			arg_cnt++;	
			token = strtok(NULL, " ");
		}
		
		// Add extra NULL required by exec. 
		if (save_token(&cmd, NULL, arg_cnt)) {
			// do something
			exit(EXIT_FAILURE);
		}
				
		//--------------------------------------------------
		
		// Save the parsed command and arguments.        
		progcmds = realloc(progcmds, (proc_cnt+1)*sizeof(command));
		if (progcmds == NULL) {
			perror("realloc error");
			exit(EXIT_FAILURE);
		}
		progcmds[proc_cnt++] = cmd;
	}
	

*/










/**
 * Dynamically allocate memory. Same as malloc(), but exits on error.
 *
 * @param size	Number of bytes to allocate.
 * @return		Pointer to allocated memory.
 */
void *safe_malloc(size_t size)
{
	void *m = malloc(size);
	if (m == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	return m;
}



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
 * Open a specified number of pipes.
 *
 * @param n			The number of pipes to create.
 * @param pipeID	Pointer to the created 2D array of pipe file descriptors. 
 * @return			0 upon successful creation of all pipes.
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
 * For a given child process, duplicate pipe file descriptors to stdin/stdout.
 *
 * @param proc_cnt  The total number of children processes.
 * @param child_no  Unique identifier of the child, giving the relative order.
 * @param pipeID    2D array of pipe file descriptors.
 * @return			0 opon successful duplication.
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
	if (pipeID) {
		for (int i = 0; i < pipe_cnt; i++) {
			if (pipeID[i]) {
				close(pipeID[i][READ_END]);
				close(pipeID[i][WRITE_END]);
			}
		}	
	}
}


/**
 * Execute a program with arguments.
 *
 * @param argv	Array of pointers to null-terminated strings, representing program arguments.
 * @return 		Returns 1 if exec fails.
 */
int exec_cmd(char *argv[]) 
{
	if (execvp(argv[0], argv) < 0) {
		perror(argv[0]);
		return 1;
	}
	return 0; // Never executed, but the compiler complains if line is removed. 	
}


/**
 * Wait on a child of the calling process. Same as wait(), but checks errors.
 * 
 * @return 0 on success, 1 on wait error or if child exited with errors.
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
 * Let the process (assumed to be parent) wait on all its children.
 *
 * @param proc_cnt  The number of children processes.
 * @return          Returns 0 if successfully waited on all children.
 */
int wait_on_children(int proc_cnt) 
{
 	for (int i = 0; i < proc_cnt; i++) {
 		if (safe_wait()) {
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
void kill_all(int proc_cnt, int **pipeID, list *prog_cmds) 
{
	
	
	/* Freeing dynamically allocated memory */ 
	
	
	command *next;
	command *p = prog_cmds->head;
	
	while (p->next != NULL) {
		char **cmd = p->next->argv;
		for (int i = 0; cmd[i] != NULL; i++) {
			free(cmd[i]);
		}
		next = p->next;
		free(p);
		p = next;
	}
	free(p);
	free(prog_cmds);
	
	/*
	for (int i = 0; i < proc_cnt; i++) {
		int j = 0; 
		while (prog_cmds[i][j] != NULL) {
			free(prog_cmds[i][j]);
			j++;
		}
		free(prog_cmds[i]);
	}
	free(prog_cmds);
	*/

	// --------
	if (pipeID) {
		for (int i = 0; i < proc_cnt-1; i++) {
			free(pipeID[i]);
		}	
	}
	free(pipeID);
}

