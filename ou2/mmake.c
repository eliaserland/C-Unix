/*
 * mmake - Automated building of programs. A barebones self-crafted version of 
 *         the GNU make utility.
 *
 * OU2 for C programming and Unix, Umea University Autumn 2021.
 * 
 * Author: Elias Olofsson (tfy17eon@cs.umu.se) 
 *
 * Version information:
 *   2021-10-01: v1.0, first public version.   
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "parser.h"
#include "stack.h"

int main (int argc, char *argv[]) {
    
    bool force_build = false;
    bool silent_mode = false;
    bool custom_mkfile = false;
    int opt;    
    char *mkfile;
    
    /* Parse options. */
    while ((opt = getopt(argc, argv, "f:Bs")) != -1) {
    	switch(opt) {
    	case 'f':
    		if ((mkfile = strdup(optarg)) == NULL) {
  				perror("strdup error");
  				exit(EXIT_FAILURE);
    		}
    		custom_mkfile = true;
    		break;
    	case 'B':
    		force_build = true;
    		break;
    	case 's':
	    	silent_mode = true; 
    		break;
    	default: 
			fprintf(stderr, "Usage: %s [-f MAKEFILE] [-B] [-s] [TARGET...]\n",
			        argv[0]);
			exit(EXIT_FAILURE);
    	}
    }
    
    /* Parse custom targets. */
    char **targets = NULL;
    if (optind < argc) {
    	targets = malloc((argc-optind)*sizeof(char*));
    	if (targets == NULL) {
    		perror("malloc error");
			exit(EXIT_FAILURE);
    	}
    	for (int i = 0; i < argc-optind; i++) {
    		if ((targets[i] = strdup(argv[i+optind])) == NULL) {
    			perror("strdup error");
  				exit(EXIT_FAILURE);
    		}
    	}
    }
        
    /* Open and parse the mmakefile. */
    FILE *mmakefp;
    if (!custom_mkfile) {
    	mkfile = "mmakefile";
	} 
    if ((mmakefp = fopen(mkfile, "r")) == NULL) {
    	perror(mkfile);
		exit(EXIT_FAILURE);
    }
    struct makefile *m = parse_makefile(mmakefp);
    if (m == NULL) {
    	fprintf(stderr, "%s: Parsing error\n", mkfile);
    	exit(EXIT_FAILURE);
    }
    fclose(mmakefp); 
	
    
    
    char *target = makefile_default_target(m);
    
    // create empty stack
    
    // if no custom targets:
    	// place default target on stack
    // else:
    	// place all custom targets on stack
    	
    // while stack is not empty:
    	// take a target from the stack
    	
		// if target does not exist
			// place all prereqs on stack		    	
    	
    	// get prereqs
    	// for each prereq 
    	
    		// if does not exist
 	   			// place prereq on the stack
    		// if newer than target
    			// place prereq on the stack
    		// if force_build 
    			// place prereq on the stack
    			
    		
    			
    		
    		
    
    
    
    
    
    
    
    
    // IF no custom targets
    	// get default target
    	
    // Else
    	// For each custom target
    	
    
    // for each target
    	
    	// execute command if force_build OR prereq newer than target OR target does not exist
    	
    	
    
    
    
    
    
    
    
    
    
    // TEST PRINTS
    for (int i = 0; i < argc-optind; i++) {
    	printf("targets[%d]=%s\n", i, targets[i]);
    }
    
    
    
    
    
    
    
    
    /* Free dynamically allocated memory */
    if (custom_mkfile) {
    	free(mkfile);
    }
    if (targets != NULL) {
		for (int i = 0; i < argc-optind; i++) {
    		free(targets[i]);
    	}
    	free(targets);
    }
    makefile_del(m);

    return 0;
} 






