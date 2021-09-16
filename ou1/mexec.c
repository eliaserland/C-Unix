/*
 * mexec - Execute an arbitrary pipeline. 
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
//#include <sys/types.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1
#define BUFSIZE 1024

//void visualize_args(int argc, char *argv[]);


int main (int argc, char *argv[]) {
    
    //visualize_args(argc, argv); // REMOVE THIS
    FILE *input;
    
    if (argc == 1) { 
        // Read from stdin
        input = stdin;
    } else if (argc == 2) {
        // Read from file
        if ((input = fopen(argv[1], "r")) == NULL) {
            perror("fopen error");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Too many arguments. mexec takes either one or zero arguments.\n");
        exit(EXIT_FAILURE);
    }
    
    int i, p_count = 0;
    char buffer[BUFSIZE], *token, **args;
    char ***progs = NULL;  
    
    /* Read from the specified stream, one line at a time, until EOF. */
    while (fgets(buffer, BUFSIZE, input) != NULL) {
        i = 0;
        args = NULL;
        buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline char. //VERIFY FUNCTIONALITY
        
        /* Split string into individual arguments, delimited by whitespace. */
        token = strtok(buffer, " ");
        while (token != NULL) {
            args = realloc(args, (i+1)*sizeof(char*));
            if (args == NULL) {
            	perror("realloc error");
            	exit(EXIT_FAILURE);
            }
            if ((args[i++] = strdup(token)) == NULL) {
            	perror("strdup error");
            	exit(EXIT_FAILURE);
            }
            token = strtok(NULL, " ");
        }
        
        /* Add extra NULL required by exec. */
        args = realloc(args, (i+1)*sizeof(char*));
        if (args == NULL) {
    		perror("realloc error");
        	exit(EXIT_FAILURE);
        }
		args[i] = (char *) NULL; 
        
		/* Save args */        
		progs = realloc(progs, (p_count+1)*sizeof(char**));
		if (progs == NULL) {
			perror("realloc error");
        	exit(EXIT_FAILURE);
		}
		progs[p_count++] = args;
    }
    fclose(input);
    
    /* Open the (p_count-1) pipes required. */
    int pipeID[p_count-1][2];
    for (int i = 0; i < p_count-1; i++) {
    	if (pipe(pipeID[i]) != 0) {
    		perror("pipe error");
        	exit(EXIT_FAILURE);
    	}
    } 
    
    /* Create the (p_count) children processes */
    int child = -1;
    pid_t pid; 
    for (int i = 0; i < p_count; i++) {
    	pid = fork();
    	if (pid < 0) {
    		perror("fork error");
        	exit(EXIT_FAILURE);
    	} else if (pid == 0) { // CHILD
    		
    		/* Perform dup2 */
    		if (i == 0) { 
    			// First child
				dup2(pipeID[i][WRITE_END], STDOUT_FILENO); 	// ERROR HANDLING
    		} else if (i == p_count-1) { 
    			// Last child
    			dup2(pipeID[i-1][READ_END], STDIN_FILENO); 	// ERROR HANDLING
    		} else { 
    			// Intermediate children
    			dup2(pipeID[i-1][READ_END], STDIN_FILENO); 	// ERROR HANDLING
	    		dup2(pipeID[i][WRITE_END], STDOUT_FILENO); 	// ERROR HANDLING
    		}
    		child = i; 
    		break;    	
    	}
    }
    
    /* Close all pipes (parent & children). */
    for (int i = 0; i < p_count-1; i++) {
    	for (int j = 0; j < 2; j++) {
    		close(pipeID[i][j]);
    	}
    }
    
    /*
    // TEST PRINT
    for (int i = 0; i < p_count; i++) {
    	int j = 0; 
    	while (progs[i][j] != NULL) {
    		printf("%s ", progs[i][j]);
    		j++;
    	}
    	printf("\n");
    }
    */
    
    
    /* Exec on all children. */
    if (child != -1) {
    	if (execvp(progs[child][0], progs[child]) < 0) {
    		perror("execv error");
    		exit(EXIT_FAILURE);
    	}
    } 
    
    /* Wait on all children. */
    int status;
 	for (int i = 0; i < p_count; i++) {
 		if ((pid = wait(&status)) == -1) {
 			perror("wait error");
 			exit(EXIT_FAILURE);
 		}
 		if (WEXITSTATUS(status) != 0) {
 			fprintf(stderr, "Child terminated with error\n");
        	exit(EXIT_FAILURE);
 		}
 	}   
    
   
    
    
    // 2D INDEXING IN "progs" IS F-ED, NEED TO BE SQUARE TO WORK ???
    

    
    // Parse input arguments
    
    // Open all pipes required
    
    // Fork, create all childs
    
    // dup2
    
    // Close all unused pipe-ends 
    
    // Exec in each child
    
    // Wait

    // Freeing dynamic memory 
    for (int i = 0; i < p_count; i++) {
    	int j = 0; 
    	while (progs[i][j] != NULL) {
    		free(progs[i][j]);
    		j++;
    	}
    	free(progs[i]);
    }
    free(progs);
    
    return 0;
}


/*
void visualize_args(int argc, char *argv[]) { 
    printf("argc: %d", argc);
    for (int i = 0; i < argc; i++) {
        printf(", argv[%d]: %s", i, argv[i]);
    }
    printf("\n");
}
*/

