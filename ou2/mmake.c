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


void exec_rule(makefile *make, const char *target, const char *argv[]);
void exec_rule2(makefile *make, struct rule *r, const char *target, const char *argv[]); 


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
    makefile *m = parse_makefile(mmakefp);
    if (m == NULL) {
    	fprintf(stderr, "%s: Could not parse makefile\n", mkfile);
    	exit(EXIT_FAILURE);
    }
    fclose(mmakefp); 
	
    
    const char *target = makefile_default_target(m);
    
    // create empty stack
    
    // if no custom targets:
    	// place default target on stack
    // else:
    	// place all custom targets on stack
   
	
	// while stack is not empty:
		// top & pop a target from the stack (i.e. a rule)
		// get rule associated with target 
		
		// if rule exist
			// exec_rule()
		// else
			// Error message and continue (???)
   
   
   // ----------------------
   
   
    // while stack is not empty:
    	
    	// check target at the top of the stack
    	
    	// for all prereqs:
    		// if rule exists, add to stack
    		// if file  
    	
    	
    	
    	
    	
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
    			
    		
    
    // ------------------------------------------
        
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


/*
void exec_rule(makefile *make, const char *target, const char *argv[]) {
	rule *r;
	char *p;
	const char **prereqs;
	r = makefile_rule(make, target); 
	if (r == NULL) {
		// NO RULE WITH NAME
		if (access(target, F_OK) != 0) {
			fprintf(stderr, "%s: No rule to make target '%s'", argv[0], target);
			exit(EXIT_FAILURE);
		} else {
			// NO RULE, BUT FILE EXIST	
			// ---> 			
		}
	} else {
		// RULE WITH NAME
		// Get prerequisites of rule
		prereqs = rule_prereq(r);
		// For all prereqs:
		for (int i = 0; prereqs[i] != NULL; i++) {
			exec_rule(make, prereqs[i], argv);		
		}
		char **cmd = rule_cmd(r);
		
		if force_build OR target does not exist OR any prereq newer than target:
			// Execute command
				
	}	
}	
*/




void exec_rule2(makefile *make, struct rule *r, const char *target, const char *argv[]) {

	// RULE r IS ASSUMED TO BE VALID AND EXISTING

	// get prereqs of rule
	const char **prereqs = rule_prereq(r);
	
	// for all prereqs:
	for (int i = 0; prereqs[i] != NULL; i++) {
		rule *prereq_rule = makefile_rule(make, prereqs[i]);
		if (prereq_rule != NULL) {
			// Rule for the prerequisite exists.
			exec_rule2(make, prereq_rule, prereqs[i], argv);
		} else {
			if (access(prereqs[i], F_OK) != 0) {
				// Prereq file not existing & also no rule to create it.
				fprintf(stderr, "%s: No rule to make target '%s', " 
				        "needed by '%s'.\n", argv[0], prereqs[i], target);
				exit(EXIT_FAILURE);
			}
			// No rule, but prequisite file exist.	
		}	
		// The rule for the prereq has been run OR there was no rule, but the file exist.
		// --> prereq satisfied		
	}
	// all prereqs satisfied
	
	char **cmd = rule_cmd(r);
	/*	
	if force_build OR target file does not exist OR any prereq file newer than target file {
		// Execute command
	}
	*/
}	


