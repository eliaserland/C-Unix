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


typedef char **command; 



int safe_wait(void);


void *safe_malloc(size_t size);

int safe_strdup(const char *s, char **s2);


int safe_realloc(void *ptr, size_t size); 


int open_pipe(int *p[]);
int open_pipes(int n, int ***pipeID);
int  spawn_children		(int proc_cnt, int *child_no);
int  dup_pipe			(int proc_cnt, int child_no, int **pipeID);
void close_pipes		(int pipe_cnt, int **pipeID);

int exec_cmd(command cmd);

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
	
	command *cmds = NULL;
	 
	
	/* Read from the specified stream, one line at a time, until EOF. */
	while (fgets(buffer, BUFSIZE, input) != NULL) {
		
		//--------------------------------------------------
		buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline character.
		arg_cnt = 0;
		args = NULL;
		
		/* Split buffer string into arguments, delimited by whitespace. */
		token = strtok(buffer, " ");
		while (token != NULL) {
			/*
			if (safe_realloc(&args, (arg_cnt+1)*sizeof(char*))) {
				fprintf(stderr, "safe_realloc failed\n");
				exit(EXIT_FAILURE);
			}*/
			
	
			args = realloc(args, (arg_cnt+1)*sizeof(char*));
			if (args == NULL) {
				perror("realloc error");
				exit(EXIT_FAILURE);
			}
			
			/*if (safe_strdup(token, &args[arg_cnt++])) {
				exit(EXIT_FAILURE);
			}*/
				
			
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
		
		//--------------------------------------------------
		
		
		/* Save the parsed command and arguments. */        
		cmds = realloc(cmds, (proc_cnt+1)*sizeof(command));
		if (cmds == NULL) {
			perror("realloc error");
			exit(EXIT_FAILURE);
		}
		cmds[proc_cnt++] = args;
		
		
		
	}
	if (argc == 2) {
		fclose(input); // Only close stream if reading from file.
	}
	
	fprintf(stderr, "BOOYA!\n");
	
	
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
		kill_all(proc_cnt, pipeID, cmds);	
		exit(EXIT_FAILURE);
	}
	


	// -------- Can use kill_all() from here on ------------
	
	/* Create the (proc_cnt) children processes */
	int child_no;
	if (spawn_children(proc_cnt, &child_no) != 0) {
		close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, cmds);
		exit(EXIT_FAILURE);
	}
	
	/* Duplicate pipe file descriptors to stdin/stdout appropriately for all children. */
	if (pipes && child_no > -1 && dup_pipe(proc_cnt, child_no, pipeID) != 0) {
		close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, cmds);
		exit(EXIT_FAILURE);
	}  
	
	/* Close all pipes (parent & children). */
	close_pipes(proc_cnt-1, pipeID);
	   
	/* Exec on all children. */
	if (child_no > -1 && exec_cmd(cmds[child_no]) != 0) {
		//close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, cmds);
		exit(EXIT_FAILURE);
	} 
	
	/* Let parent wait on all children. */	
	if (wait_on_children(proc_cnt)) {
		//close_pipes(proc_cnt-1, pipeID);
		kill_all(proc_cnt, pipeID, cmds);
		exit(EXIT_FAILURE);
	}

	//close_pipes(proc_cnt-1, pipeID);

	/* Free dynamically allocated memory */
	kill_all(proc_cnt, pipeID, cmds);
	
		
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



/*
int safe_realloc(void *ptr, size_t size) 
{
	void **p = (void**)ptr;
	
	void **ptr_new = realloc(*p, size);
	if (ptr_new == NULL) {
		perror("realloc error");
		return 1;
	}
	*ptr = ptr_new;
	return 0;
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
 * @param cmd	Array of strings, representing a program command with arguments.
 * @return 		Returns 1 if exec fails.
 */
int exec_cmd(command cmd) 
{
	if (execvp(cmd[0], cmd) < 0) {
		perror(cmd[0]);
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
void kill_all(int proc_cnt, int **pipeID, command *prog_cmds) 
{
	
	
	/* Freeing dynamically allocated memory */ 
	
	/*
	if (prog_cmds) {
		
	
	}*/
	
	for (int i = 0; i < proc_cnt; i++) {
		int j = 0; 
		while (prog_cmds[i][j] != NULL) {
			free(prog_cmds[i][j]);
			j++;
		}
		free(prog_cmds[i]);
	}
	free(prog_cmds);
	

	// --------
	if (pipeID) {
		for (int i = 0; i < proc_cnt-1; i++) {
			free(pipeID[i]);
		}	
	}
	free(pipeID);
}

