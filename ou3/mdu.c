/*
 * mdu - Estimate file space usage. A barebones self-crafted version of
 *       the GNU du utility.
 *
 * OU3 for C programming and Unix, Umea University Autumn 2021.
 *
 * Author: Elias Olofsson (tfy17eon@cs.umu.se)
 *
 * Version information:
 *   2021-10-15: v1.0, first public version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "list.h"


void print_usage(char *program_name);
char *safe_strdup(const char *s);
void *thread_worker(void *args);
//blkcnt_t get_file_blkcnt(char *filename);
//blkcnt_t get_dir_blkcnt(char *basepath);

blkcnt_t blockcount(char *filename);
blkcnt_t blockcount_dir(char *basepath);

int main (int argc, char *argv[])
{
	// Parse program arguments. 
	int opt;
	int num_threads;
	while ((opt = getopt(argc, argv, "j:")) != -1) {
		switch(opt) {
		case 'j':
			num_threads = atoi(optarg);
			if (num_threads < 1) {
				fprintf(stderr, "Warning: NUM_THREADS must be greater than or equal to 1.\n");
				exit(EXIT_FAILURE);
			}
			break;
		default:
			print_usage(argv[0]);
		}

	}
	if (optind == argc) {
		// No files or folders were given as aguments.
		print_usage(argv[0]);
	}


	char *s;
	list *files = list_empty(NULL);

	

	// We have additional file targets given as argument
	for (int i = 0; i < argc-optind; i++) {
		
		s = strdup(argv[i+optind]);
		if (s == NULL) {
			perror("strdup");
			exit(EXIT_FAILURE);
		}
		if (list_append(files, s)) {
			exit(EXIT_FAILURE);
		}
	}
	
	while (!list_is_empty(files)) {
		char *s2 = list_pop(files);
		//printf("%s\n", s2);
		//get_blkcnt(s2);
		printf("%ld %s\n", blockcount(s2), s2);

		free(s2);
	}


	// Declare/Initialize mutexes, condition variables, threads.

	// Create data structures: Thread pool, task stack/queue

	// For each thread: start thread algorithm.

	// In main thread: start thread algorithm.

	// Join all threads.

	// Clean up all memory.
	/*
	list_pos p = list_first(l);
	while(!list_is_end(l, p)) {
		free(list_inspect(l, p));
		p = list_next(l, p);
	}
	*/
	list_kill(files);
		
	printf("END!\n");
	return 0;
}

/**
 * print_usage() - Print program synopsis and exit.
 * @param program_name String with the name of the program.		.
 */
void print_usage(char *program_name) 
{
	fprintf(stderr, "Usage: %s [-j NUM_THREADS] FILE [FILES...]\n",
	        program_name);
	exit(EXIT_FAILURE);
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


void *thread_worker(void *args) 
{
	// Typecast args, split
	while (1) {

	}
}


/**
 * 
 *
 * @param file		String containing the filename.
 * @return			true if file exists.
 */
/*
blkcnt_t get_blkcnt(char *filename) 
{
	blkcnt_t tot_cnt;
	struct stat s;
	DIR *dir;
	struct dirent *dir_ptr;

	if (lstat(filename, &s) == -1) {
		perror("lstat");
		return (blkcnt_t)-1;
	}
	tot_cnt = s.st_blocks;
	*/
	/*
	char *cwd = malloc(256*sizeof(char));
	getcwd(cwd, 256);
	printf("%s\n", cwd);

	dir = opendir(cwd);
	if (dir == NULL) {
		perror("opendir");
		// do something
	}
	return 1;
	*/
	/*
	// Check if file is a directory.
	if (S_ISDIR(s.st_mode)) {
		// Open directory.
		dir = opendir(filename);
		if (dir == NULL) {
			perror("opendir");
			// do something
		}

		while ((dir_ptr = readdir(dir)) != NULL) {
			printf("%s\n", dir_ptr->d_name);
			//get_blkcnt(strdup(dir_ptr->d_name));
		}

		closedir(dir);
		// For each item in the directory:
			// If not a directory:
				// Get blockcount and add to total.
			// If item is a directory:

				// MUTEX LOCK
				// Add subdirectory to task list.
				// BROADCAST/SIGNAL
				// MUTEX UNLOCK

	} 

	return tot_cnt;

}
*/










	// Change current working directory

	// opendir(".")

	// For each file in CWD:
		// Get filename

		// lstat

		// save blockcount

		// if file is directory
			// Add absolute path to task list

		
/**
 * 
 *
 * @param file		String containing the filename.
 * @return			true if file exists.
 */
blkcnt_t blockcount(char *filename) 
{
	blkcnt_t tot;
	struct stat s;
	if (lstat(filename, &s) == -1) {
		perror("lstat");
		return 0; //(blkcnt_t)-1;
	}
	tot = s.st_blocks;

	//printf("%ld %s\n", s.st_blocks, filename);

	if (S_ISDIR(s.st_mode)) {
		// MUTEX LOCK
		// Add subdirectory to task list.
		// BROADCAST/SIGNAL
		// MUTEX UNLOCK
		tot += blockcount_dir(filename);
	}
	return tot;
}

blkcnt_t blockcount_dir(char *basepath)
{
	blkcnt_t tot = 0;
	DIR *dir;
	struct dirent *dir_ptr;

	// Open directory.
	dir = opendir(basepath); // Absolute or relative
	if (dir == NULL) {
		perror("opendir");
		// do something
	}

	char *path = strdup(basepath);
	int base_len = strlen(basepath)+1;
	char *tmp;

	while ((dir_ptr = readdir(dir)) != NULL) {
		//printf("%s\n", dir_ptr->d_name);
		if (strcmp(dir_ptr->d_name, ".") == 0 || strcmp(dir_ptr->d_name, "..") == 0) {
			continue;
		}

		tmp = realloc(path, base_len + strlen(dir_ptr->d_name) + 1);
		if (tmp == NULL) {
			perror("realloc");
		}
		path = tmp;

		strcpy(path, basepath);
		strcat(path, "/");
		strcat(path, dir_ptr->d_name);


		//printf("%s\n", path);

		tot += blockcount(path);

	}

	closedir(dir);

	return tot;
}






















