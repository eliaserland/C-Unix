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

void close_pipes(int pipe_cnt, int **pipeID);
int exec_cmd(char **args);
int wait_on_children(int proc_cnt);
void kill_all(int proc_cnt, int **pipeID, char ***prog_cmds);


int main (int argc, char *argv[]) {
	
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
	if (proc_cnt > 1) {
		pipeID = malloc((proc_cnt-1)*sizeof(int*));
		if (pipeID == NULL) {
			perror("malloc error");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < proc_cnt-1; i++) {
			pipeID[i] = malloc(2*sizeof(int));
			if (pipeID[i] == NULL) {
				perror("malloc error");
				exit(EXIT_FAILURE);
			}
			if (pipe(pipeID[i]) != 0) {
				perror("pipe error");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	// -------- Can use kill_all() from here on ------------
	
	spawn_children(proc_cnt)
	
	/* Create the (proc_cnt) children processes */
	int child_no;
	pid_t pid; 
	for (int i = 0; i < proc_cnt; i++) {
		pid = fork();
		if (pid < 0) {
			perror("fork error");
			exit(EXIT_FAILURE);
		} else if (pid == 0) { // Only children
			/* Perform dup2 */
			if (proc_cnt > 1) {	
				if (i == 0) { 
					// First child
					if (dup2(pipeID[i][WRITE_END], STDOUT_FILENO) == -1) {
						perror("dup2 error");
						exit(EXIT_FAILURE);
					}
				} else if (i == proc_cnt-1) { 
					// Last child
					if (dup2(pipeID[i-1][READ_END], STDIN_FILENO) == -1) {
						perror("dup2 error");
						exit(EXIT_FAILURE);
					}
				} else { 
					// Intermediate children
					if (dup2(pipeID[i-1][READ_END], STDIN_FILENO) == -1) {
						perror("dup2 error");
						exit(EXIT_FAILURE);
					}
					if (dup2(pipeID[i][WRITE_END], STDOUT_FILENO) == -1) {
						perror("dup2 error");
						exit(EXIT_FAILURE);
					}
				}
			}
			child_no = i; 
			break;    	
		}
	}
	
	/* Close all pipes (parent & children). */
	close_pipes(proc_cnt, pipeID);
	   
	/* Exec on all children. */
	if (pid == 0 && exec_cmd(prog_cmds[child_no]) != 0) {
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
 * Create a specified number of chilren processes.
 *
 * @param proc_cnt  The number of children to create.
 * @param child_no  A pointer to a .
 */
int spawn_children(int proc_cnt, int *child_no) {
	pid_t pid;	
	for (int i = 0; i < proc_cnt; i++) {
		pid = fork();
		if (pid < 0) {
			perror("fork error");
			return 1
		} else if (pid == 0) { // Only children
			*child_no = i; 
			break;
		}	
	}
	return 0;
}
	
	/* Create the (proc_cnt) children processes */
	int child_no;
	pid_t pid; 
	for (int i = 0; i < proc_cnt; i++) {
		pid = fork();
		if (pid < 0) {
			perror("fork error");
			exit(EXIT_FAILURE);
		} else if (pid == 0) { // Only children
			/* Perform dup2 */
			if (proc_cnt > 1) {	
				if (i == 0) { 
					// First child
					if (dup2(pipeID[i][WRITE_END], STDOUT_FILENO) == -1) {
						perror("dup2 error");
						exit(EXIT_FAILURE);
					}
				} else if (i == proc_cnt-1) { 
					// Last child
					if (dup2(pipeID[i-1][READ_END], STDIN_FILENO) == -1) {
						perror("dup2 error");
						exit(EXIT_FAILURE);
					}
				} else { 
					// Intermediate children
					if (dup2(pipeID[i-1][READ_END], STDIN_FILENO) == -1) {
						perror("dup2 error");
						exit(EXIT_FAILURE);
					}
					if (dup2(pipeID[i][WRITE_END], STDOUT_FILENO) == -1) {
						perror("dup2 error");
						exit(EXIT_FAILURE);
					}
				}
			}
			child_no = i; 
			break;    	
		}
	}


/**
 * Close all pipes.
 *
 * @param pipe_cnt  The number of open pipes.
 * @param pipeID    2D array of pipe file descriptors.
 */
void close_pipes(int pipe_cnt, int **pipeID) {
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
int exec_cmd(char **args) {
	if (execvp(args[0], args) < 0) {
		perror(args[0]);
		return 1;
	}	
}

/**
 * Let the process (assumed to be parent) wait on all its children.
 *
 * @param proc_cnt  The number of children processes.
 * @return 			Returns 0 if successfully waited on all children.
 */
int wait_on_children(int proc_cnt) {
	int status;
 	for (int i = 0; i < proc_cnt; i++) {
 		if ((wait(&status)) == -1) {
 			perror("wait error");
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
void kill_all(int proc_cnt, int **pipeID, char ***prog_cmds) {
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



