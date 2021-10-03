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

typedef struct cmd_array {
	char **args;
} cmd_array;



//int safe_wait(void);


//void *safe_malloc(size_t size);

//int safe_strdup(const char *s, char **s2);


//int safe_realloc(void *ptr, size_t size); 


//int open_pipe(int *p[]);
//int open_pipes(int n, int ***pipeID);
//int  spawn_children		(int proc_cnt, int *child_no);
//int  dup_pipe			(int proc_cnt, int child_no, int **pipeID);
//void close_pipes		(int pipe_cnt, int **pipeID);
//int  exec_cmd			(char **args);
//int  wait_on_children	(int proc_cnt);
//void kill_all			(int proc_cnt, int **pipeID, char ***prog_cmds);



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
	cmd_array *cmds = NULL;
	 
	for (int i = 0; i < 5; i++) {
		cmds = realloc(cmds, (i+1)*sizeof(cmd_array));
		if (cmds == NULL) {
			perror("realloc");
			exit(EXIT_FAILURE);
		}
	}
	
	
	fprintf(stderr, "GOT HERE!\n");
	
		
	return 0;
}



