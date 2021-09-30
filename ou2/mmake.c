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

void *safe_malloc(size_t size);
char *safe_strdup(const char *s);
pid_t safe_fork(void);
void safe_wait(void);
void safe_execvp(const char *file, char *const argv[]);
bool file_exists(const char *file);
time_t modtime(const char *file);
makefile *open_and_parse_mkfile(const char *mkfile);
void print_cmd(char **cmd);
void exec_cmd(char **cmd, bool silent);
bool exec_rule(makefile *make, struct rule *r, const char *target, 
               const char *prgname, bool silent, bool force);

int main (int argc, char *argv[]) 
{    
    bool force_build = false;
    bool silent_mode = false;
    bool custom_mkfile = false;
    int opt;    
    char *mkfile = NULL;
    
    /* Parse options. */
    while ((opt = getopt(argc, argv, "f:Bs")) != -1) {
    	switch(opt) {
    	case 'f':
    		mkfile = safe_strdup(optarg);
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
    makefile *m = open_and_parse_mkfile(mkfile);	
	
	/* Parse targets, custom or default. */
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
	targets = safe_malloc(target_cnt * sizeof(char*));
    if (custom_targets) {
		/* Get each custom target. */
    	for (int i = 0; i < target_cnt; i++) {
    		targets[i] = safe_strdup(argv[i+optind]);
    	}
    } else {
		/* Get the default target. */ 
		targets[0] = safe_strdup(makefile_default_target(m));
    }
    
    /* For each target, run the associated rule. */
	char *target; 
    for (int i = 0; i < target_cnt; i++) {
		target = targets[i];
   		rule *r = makefile_rule(m, target);
   		if (r == NULL) {
   			fprintf(stderr, "%s: No rule to make target '%s'.\n", 
   				argv[0], target);
			exit(EXIT_FAILURE);
   		} 
   		exec_rule(m, r, target, argv[0], silent_mode, force_build);
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

/**
 * Duplicate a string. Same as strdup(), but exits on error.
 *
 * @param file		String containing the filename.
 * @return			true if file exists.
 */
char *safe_strdup(const char *s) 
{
	char *s2;
	if ((s2 = strdup(s)) == NULL) {
		perror("strdup");
		exit(EXIT_FAILURE);
	}
	return s2;
}

/**
 * Duplicate the calling process. Same as fork(), but exits on error.
 *
 * @return PID of the child in the parent process, and 0 in the child process.
 */
pid_t safe_fork(void) 
{
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	return pid;
}

/**
 * Wait on a child of the calling process. Same as wait(), but exits on error.
 */
void safe_wait(void) 
{
	int status;
	if (wait(&status) == -1){
		perror("wait");
 		exit(EXIT_FAILURE);
	}
	if (WEXITSTATUS(status) != 0) {
		exit(EXIT_FAILURE);
 	}
}

/**
 * Replace the current process image. Same as execvp(), but exits on error.
 *
 * @param file 	Filename of program being executed. 
 * @param argv 	Array of null-terminated strings, representing arguments. 
 */
void safe_execvp(const char *file, char *const argv[])
{
	if (execvp(file, argv) < 0) {
		perror(file);
		exit(EXIT_FAILURE);
	}
}

/**
 * Check if a file exists.
 *
 * @param file		String containing the filename.
 * @return			true if file exists.
 */
bool file_exists(const char *file) 
{
	return access(file, F_OK) == 0;
}

/**
 * Get last modification time of specific file.
 *
 * @param file		String with name of file in current directory. 
 * @return			Modification time.
 */
time_t modtime(const char *file) 
{
	struct stat s;
	if (stat(file, &s) == -1) {
		perror("stat"); 
		exit(EXIT_FAILURE);
	}
	return s.st_mtime;
}

/**
 * Open and parse a makefile.
 *
 * @param mkfile   String with name of makefile, else if NULL pointer using 
 *                 default name "mmakefile".
 * @return         Parsed makefile.
 */
makefile *open_and_parse_mkfile(const char *mkfile) 
{   
	FILE *mmakefp;
	if (mkfile == NULL) {
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

/**
 * Print the executed command and associated arguments to stdout.
 *
 * @param cmd	Array of null-terminated strings, with last element being NULL. 
 */
void print_cmd(char **cmd) 
{
	printf("%s", cmd[0]);
	for (int i = 1; cmd[i] != NULL; i++) {
		printf(" %s", cmd[i]);	
	}
	printf("\n");
}

/**
 * Fork and execute a command in the child process.
 *
 * @param cmd    Array of null-terminated strings, with last element being NULL.
 * @param silent Silent mode, suppresses prints of the commands to terminal.  
 */
void exec_cmd(char **cmd, bool silent) 
{	
	if (!silent) {
		print_cmd(cmd);		
	}
	pid_t pid = safe_fork(); 		
	if (pid == 0) { 
		/* Execute the command in the child process. */
		safe_execvp(cmd[0], cmd);
	}
	/* Let parent wait on child. */
	safe_wait();
}

/**
 * Run the rule associated with a target. The rule is assumed to exist. 
 *
 * @param make    Pointer to the parsed makefile.
 * @param r       The given rule to run. Is assumed to be existing.
 * @param target  Name of the target for the given rule.
 * @param prgname Name of the program.
 * @param silent  Silent mode, suppresses prints of the commands to terminal.
 * @param force   Force the build of all targets.
 * @return 	  True if the given rule has executed its command.  
 */
bool exec_rule(makefile *make, struct rule *r, const char *target, 
               const char *prgname, bool silent, bool force) 
{	
	time_t mtime, mtime_new = 0;
	bool ret, build = false;
	const char *prereq; 
	char **cmd;	
	
	/* Get prerequisites of the rule. */
	const char **prereqs = rule_prereq(r);
	
	/* For each prerequisite, run the associated prereq 
	   rule or check if prereq file exists. */ 
	for (int i = 0; prereqs[i] != NULL; i++) {
		prereq = prereqs[i];
		rule *prereq_rule = makefile_rule(make, prereq);
		if (prereq_rule == NULL) {
			/* No rule, check if prequisite file exists. */
			if (!file_exists(prereq)) {
				fprintf(stderr, "%s: No rule to make target '%s', " 
				        "needed by '%s'.\n", prgname, prereq, target);
				exit(EXIT_FAILURE);
			}
			/* Get modification time of prereq file. */
			mtime = modtime(prereq);
			if (mtime > mtime_new) {
				mtime_new = mtime;
			}
		} else {
			/* Prerequisite rule does exist, run it first. */
			ret = exec_rule(make, prereq_rule, prereq, prgname, silent, force);
			if (ret) {
				build = true; 
			}				
		}
	}
	/* By now, all prerequisites should be satisfied. */

	/* Execute the command of the rule, if any of the following are satisfied:
	    - Force build option is chosen.
		- Target file does not exist.
		- Any prereq file is newer than target file.
		- Any prereq rule have executed their command. */
	if (force || build || !file_exists(target) || modtime(target) < mtime_new) {
		cmd = rule_cmd(r);
		exec_cmd(cmd, silent);
		return true;
	}
	return false;
}	
