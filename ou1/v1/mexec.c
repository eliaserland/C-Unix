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
	for (int i = 0; i < proc_cnt-1; i++) {
		for (int j = 0; j < 2; j++) {
			close(pipeID[i][j]);
		}
	}
	   
	/* Exec on all children. */
	if (pid == 0) {
		if (execvp(prog_cmds[child_no][0], prog_cmds[child_no]) < 0) {
			perror(prog_cmds[child_no][0]);
			exit(EXIT_FAILURE);
		}
	} 
	
	/* Let parent wait on all children. */
	int status;
 	for (int i = 0; i < proc_cnt; i++) {
 		if ((pid = wait(&status)) == -1) {
 			perror("wait error");
 			exit(EXIT_FAILURE);
 		}
 		if (WEXITSTATUS(status) != 0) {
 			fprintf(stderr, "Child process exited with error.\n");
			exit(EXIT_FAILURE);
 		}
 	}   

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
	
	return 0;
}
