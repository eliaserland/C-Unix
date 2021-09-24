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
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include "parser.h"


//#include "stack.h"
void print_cmd(char **cmd);
time_t get_mtime(const char *file);
bool file_exists(const char *file);
makefile *open_and_parse_mkfile(const char *mkfile, bool custom_mkfile);
void exec_cmd(char **cmd, bool silent);
void exec_rule(makefile *make, struct rule *r, const char *target, const char *prgname, bool silent, bool force_build);


// Get last modification time of file
time_t get_mtime(const char *file) {
	struct stat s;
	if (stat(file, &s) == -1) {
		perror("stat"); 
		exit(EXIT_FAILURE);
	}
	return s.st_mtime;
}

// Returns true if file exists
bool file_exists(const char *file) {
	return access(file, F_OK) == 0;
}


/* Open and parse a makefile. */
makefile *open_and_parse_mkfile(const char *mkfile, bool custom_mkfile) {   

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
	return m;
}

void print_cmd(char **cmd) {
	printf("%s", cmd[0]);
	for (int i = 1; cmd[i] != NULL; i++) {
		printf(" %s", cmd[i]);	
	}
	printf("\n");
}


void exec_cmd(char **cmd, bool silent) {
	
	if (!silent) {
		print_cmd(cmd);		
	}
	pid_t pid = fork(); 		
	if (pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) { 
		// Execute command in child process only
		if (execvp(cmd[0], cmd) < 0) {
			perror(cmd[0]);
			exit(EXIT_FAILURE);
		}
	}
	// Parent
	int status;
	if (wait(&status) == -1){
		perror("wait");
 		exit(EXIT_FAILURE);
	}
	if (WEXITSTATUS(status) != 0) {
		exit(EXIT_FAILURE);
 	}
}


void exec_rule(makefile *make, struct rule *r, const char *target, const char *prgname, bool silent, bool force_build) {
	// RULE r IS ASSUMED TO BE VALID AND EXISTING
	
	time_t mtime, mtime_newest = 0;
	
	// Get prerequisites.
	const char **prereqs = rule_prereq(r);
	
	// For each prerequisite, run associated rule or check if file exists.  
	for (int i = 0; prereqs[i] != NULL; i++) {
		rule *prereq_rule = makefile_rule(make, prereqs[i]);
		if (prereq_rule == NULL) {
			// Check if prequisite file exists.
			if (!file_exists(prereqs[i])) {
				fprintf(stderr, "%s: No rule to make target '%s', " 
				        "needed by '%s'.\n", prgname, prereqs[i], target);
				exit(EXIT_FAILURE);
			}
			
			mtime = get_mtime(prereqs[i]);
			if (mtime > mtime_newest) {
				mtime_newest = mtime;
			} 
		} else {
			// Run the prerequisite rule.
			exec_rule(make, prereq_rule, prereqs[i], prgname, silent, force_build);				
		}
		// Prerequisite rule has been run, or prereq file exists --> prereq satisfied.		
	}
	
	char **cmd = rule_cmd(r);
	
	// if force_build OR target file does not exist OR any prereq file newer than target file {
	if (force_build || !file_exists(target) || get_mtime(target) < mtime_newest) {
		exec_cmd(cmd, silent);
	} 
}	



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
  				perror("strdup");
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
    
    /* Open and parse the makefile. */
    makefile *m = open_and_parse_mkfile(mkfile, custom_mkfile);	
	
	//--------------------------------------
    
	/* Parse targets. */
	//void parse_targets(argc, argv, target_cnt, targets)
    /* Parse custom targets. */
    // argc, argv 
    
    //ret: target_cnt, targets 
    
    int target_cnt;
    bool custom_targets;
    char **targets;
    if (optind < argc) {
    	custom_targets = true; 
    	target_cnt = argc-optind;
    } else {
    	custom_targets = false;
    	target_cnt = 1;
    }
    targets = malloc(target_cnt * sizeof(char*));
	if (targets == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
    if (custom_targets) {
    	for (int i = 0; i < target_cnt; i++) {
    		if ((targets[i] = strdup(argv[i+optind])) == NULL) {
    			perror("strdup");
  				exit(EXIT_FAILURE);
    		}
    	}
    }
	/* If no targets specified, get default target. */ 
    if (!custom_targets) {
		targets[0] = strdup(makefile_default_target(m));
		if (targets[0] == NULL) {
				perror("strdup");
  				exit(EXIT_FAILURE);
		}
    }
    
    //----------------------------------------------
    
    /* For every target, run the associated rule. */
    for (int i = 0; i < target_cnt; i++) {
   		rule *r = makefile_rule(m, targets[i]);
   		if (r == NULL) {
   			fprintf(stderr, "%s: No rule to make target '%s'.\n", argv[0], targets[i]);
			exit(EXIT_FAILURE);
   		} 
   		exec_rule(m, r, targets[i], argv[0], silent_mode, force_build);
    }
    
    /* Free dynamically allocated memory */
    if (custom_mkfile) {
    	free(mkfile);
    }
	for (int i = 0; i < target_cnt; i++) {
		free(targets[i]);
	}
    free(targets);
    makefile_del(m);

    return 0;
} 

