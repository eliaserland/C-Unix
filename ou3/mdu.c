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
#include <pthread.h>

#include "list.h"

typedef struct task_item {
	char *path; // Base path of the task.
	int job_id; // Associates the task with a specific job.
} task_item;

typedef struct public_data {
	list *queue;           // Pointer to list of tasks.
	int num_jobs;          // The total number of separate jobs.
	int num_threads;       // Number of threads available.
	char **name_jobs;      // Array with length num_jobs, containing name of each job.
	blkcnt_t *blkcnt_jobs; // Array with length num_jobs, containing blockcount for each job.
	pthread_mutex_t queue_mut;   // Mutex for queue access.
	pthread_mutex_t *blkcnt_mut; // Mutexes for blockcount array access.
	pthread_cond_t cond;         // Condition variable for the queue.
} public_data;

public_data *init_public_data(int num_jobs, int num_threads);
task_item *create_task(char *path, int job_id);
int set_job_name(public_data *data, char *jobname, int job_id);
void kill_task(task_item *task);
void kill_public_data(public_data *data);


void print_usage(char *program_name);
void *thread_worker(void *args);
int run_task(public_data *data, task_item *task);
char *concatenate_paths(char *basepath, char *filename);



blkcnt_t blockcount(char *filename);
blkcnt_t blockcount_dir(char *basepath);

int main (int argc, char *argv[])
{
	// Parse program arguments. 
	int opt, num_jobs, num_threads = 1;
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
			exit(EXIT_FAILURE);
		}

	}
	// The number of additional arguments == the number of jobs (files/folders) to execute. 
	num_jobs = argc-optind;
	if (num_jobs == 0) {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	// Create data structure to hold public data.
	public_data *data = init_public_data(num_jobs, num_threads);
	if (data == NULL) {
		fprintf(stderr, "Could not initialize public data structure.\n");
		exit(EXIT_FAILURE);
	}


	// Each additional argument given&thread_worker, datais a job to execute. 
	task_item *task;
	for (int i = 0; i < num_jobs; i++) {
		
		// Create a task item.
		task = create_task(argv[i+optind], i); 
		if (task == NULL) {
			exit(EXIT_FAILURE); // Do something else?
		}

		// Set the initial task path as name for the job.
		if (set_job_name(data, task->path, i)) {
			exit(EXIT_FAILURE); // Do something else?
		}

		// Append the task to the pulic task queue.
		if (list_append(data->queue, task)) {
			exit(EXIT_FAILURE);
		}
	}
	

	// Declare/Initialize mutexes, condition variables, threads.
	pthread_t *tid = calloc(num_threads-1, sizeof(*tid));
	
	// Create data structures: Thread pool, task stack/queue

	// For each thread: start thread algorithm.
	for (int i = 0; i < num_threads-1; i++) {
		pthread_create(&tid[i], NULL, &thread_worker, data);
	}
	
	// In main thread: start thread algorithm.
	void *ret = thread_worker(data);

	// Join all threads.
	for (int i = 0; i < num_threads-1; i++) {
		pthread_join(tid[i], NULL); // NEED TO CHECK RETURN 
	}

	// Print results
	for (int i = 0; i < num_jobs; i++) {
		printf("%-7ld %s\n", data->blkcnt_jobs[i], data->name_jobs[i]);
	}

	// Clean up all resources.
	kill_public_data(data);
	free(tid);
		
	return 0;
}

/**
 * init_public_data() - Initialize and allocate the public data structure.
 *
 * @param num_jobs	The number of jobs to execute.
 * @return			Pointer to the data structure, NULL on error.
 */
public_data *init_public_data(int num_jobs, int num_threads) 
{
	list *queue;
	public_data *data = NULL;
	blkcnt_t *blkcnt_jobs = NULL;
	char **name_jobs = NULL;
	pthread_mutex_t *blkcnt_mut = NULL;


	// Create the task queue.
	queue = list_empty(NULL);
	if (queue == NULL) {
		return NULL;
	}

	do {
		// Create data structure to hold public data.
		if ((data = calloc(1, sizeof(*data))) == NULL) {
			break;
		}
		// Create array of blockcounts for each job.
		if ((blkcnt_jobs = calloc(num_jobs, sizeof(blkcnt_t))) == NULL) {
			break;
		}
		// Create array of names (i.e. file/folder) for each job.
		if ((name_jobs = calloc(num_jobs, sizeof(char *))) == NULL) {
			break;
		}
		// Allocate array of mutexes for the blockcount array.
		if ((blkcnt_mut = calloc(num_jobs, sizeof(*blkcnt_mut))) == NULL) {
			break;
		}

		// Set links in the data structure.
		data->queue = queue;
		data->blkcnt_jobs = blkcnt_jobs; 
		data->name_jobs = name_jobs;
		data->blkcnt_mut = blkcnt_mut;

		// Set the number of jobs & number of threads.
		data->num_jobs = num_jobs;
		data->num_threads = num_threads;

		// Initialize mutexes and condition variables.
		data->queue_mut = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
		for (int i = 0; i < num_jobs; i++) {
			data->blkcnt_mut[i] = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
		}
		data->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

		// Normal exit.
		return data;

	} while (0);

	// Error handling.
	perror("calloc");
	list_kill(queue);
	free(data);
	free(blkcnt_jobs);
	free(name_jobs);
	free(blkcnt_mut);
	return NULL;
}


/**
 * create_task() - Allocate and set values for a new task item.
 *
 * @param path		String with path to the root file of the task, is 
 *                  dynamically copied to the new task item.
 * @param job_id	Identfier to associate the task with a job.
 * @return			Pointer to the new task, NULL on error.
 */
task_item *create_task(char *path, int job_id) 
{
	task_item *task = NULL;
	char *new_path = NULL; 

	do {
		// Allocate memory for the new task item.
		if ((task = calloc(1, sizeof(*task))) == NULL) {
			perror("calloc");
			break;
		}
		
		// Duplicate the path string.
		if ((new_path = strdup(path)) == NULL) {
			perror("strdup");
			break;
		}

		// Set links and values.
		task->path = new_path;
		task->job_id = job_id;

		// Normal exit.
		return task;

	} while (0);

	// Error handling.
	free(task);
	free(new_path);
	return NULL;
}

/**
 * set_job_name() - Duplicate a job name to the public data structure.
 *
 * @param data		Pointer to the structure with public data to modify.
 * @param jobname	String with name to dynamically copy to the structure.
 * @param job_id	Identfier of the job to set the name of.
 * @return			Pointer to the new task, NULL on error.
 */
int set_job_name(public_data *data, char *jobname, int job_id) 
{
	char *new_name;
	if ((new_name = strdup(jobname)) == NULL) {
		perror("strdup");
		return 1;
	}
	data->name_jobs[job_id] = new_name;
	return 0;
}

/**
 * kill_task() - Return dynamically allocated memory for a task item.
 *
 * @param task		Pointer to the task item.
 */
void kill_task(task_item *task) 
{
	free(task->path);
	free(task);
}

/**
 * kill_public_data() - Return dynamically allocated memory for a the data 
 *                      structure containing public data.
 *
 * @param data 		Pointer to the structure to kill.
 */
void kill_public_data(public_data *data) 
{	
	// Kill the array of blockcounts
	free(data->blkcnt_jobs);

	// Kill the array of job names
	for (int i = 0; i < data->num_jobs; i++) {
		free(data->name_jobs[i]);		
	}
	free(data->name_jobs);

	// Kill the task queue
	while (!list_is_empty(data->queue)) {
		task_item *t = list_pop(data->queue);
		kill_task(t);
	}
	list_kill(data->queue);

	// Destroy mutexes and condition variables
	pthread_cond_destroy(&data->cond);
	pthread_mutex_destroy(&data->queue_mut);
	for (int i = 0; i < data->num_jobs; i++) {
		pthread_mutex_destroy(&data->blkcnt_mut[i]);
	}
	free(data->blkcnt_mut);

	// Kill the public data strucure itself
	free(data);
}











/**
 * print_usage() - Print program synopsis and exit.
 * @param program_name String with the name of the program.		.
 */
void print_usage(char *program_name) 
{
	fprintf(stderr, "Usage: %s [-j NUM_THREADS] FILE [FILES...]\n",
	        program_name);
}




void *thread_worker(void *args) 
{
	// Typecast args
	public_data *data = (public_data *) args;
	static int num_waiting_threads = 0; 
	static bool work_finished = false;

	while (1) {
		pthread_mutex_lock(&data->queue_mut);
		while (list_is_empty(data->queue)) {
			if (num_waiting_threads == data->num_threads-1) {
				// I am last working thread.
				work_finished = true;
				pthread_cond_broadcast(&data->cond);
				pthread_mutex_unlock(&data->queue_mut);
				return 0;
			} else if (work_finished) {
				pthread_mutex_unlock(&data->queue_mut);
				return 0;
			} else {
				num_waiting_threads++;
				pthread_cond_wait(&data->cond, &data->queue_mut);
				num_waiting_threads--;
			}
		}
		task_item *task = list_pop(data->queue);
		pthread_mutex_unlock(&data->queue_mut);
		run_task(data, task);
		kill_task(task);
	}
}

/**
 * run_task() - Execute one task item. 
 *
 * @param data		Pointer to the structure with public data.
 * @param task		Pointer to the task data structure.
 * @return			0 if the task is successfully completed, 1 on error.
 */
int run_task(public_data *data, task_item *task) 
{
	blkcnt_t blkcnt_tot = 0;
	
	struct stat sb;
	struct dirent *dir_ptr;
	DIR *dir;

	char *basepath;
	char *path;
	task_item *new_task;

	basepath = task->path;

	// Get information on the file
	if (lstat(basepath, &sb) == -1) {
		perror("lstat");
		return 1;
	}

	// Get blockcount of the root file of the task.
	blkcnt_tot += sb.st_blocks;

	// If the file is a directory. 
	// Get the blocksize of all non-directories, while marking each subdirectory as a new task.
	if (S_ISDIR(sb.st_mode)) {
		
		// Open the directory.
		dir = opendir(basepath);
		if (dir == NULL) {
			perror("opendir");
			// do something
		}
		

		// For all files in the directory:
		while ((dir_ptr = readdir(dir)) != NULL) {
			
			// Filter out the always present directories "." and ".."
			if (strcmp(dir_ptr->d_name, ".") == 0 || strcmp(dir_ptr->d_name, "..") == 0) {
				continue;
			}

			// Concatenate base path and filename
			path = concatenate_paths(basepath, dir_ptr->d_name);
			if (path == NULL) {
				//do something
			}

			// Get information on the file.
			if (lstat(path, &sb) == -1) {
				perror("lstat");
				exit(EXIT_FAILURE);
			}

			if (S_ISDIR(sb.st_mode)) {
				// For directories: Add as a new item to the task queue.
				
				// Create a new task item. Set path and let new task inherit job id.
				new_task = create_task(path, task->job_id);
				if (new_task == NULL) {
					exit(EXIT_FAILURE);
				}

				// Append new task to the task queue.
				pthread_mutex_lock(&data->queue_mut);
				if (list_append(data->queue, new_task)) {
					// ERROR
					exit(EXIT_FAILURE);
				}
				pthread_mutex_unlock(&data->queue_mut);

				// BROADCAST/SIGNAL CONDITION VARIABLE.
				pthread_cond_signal(&data->cond);

			} else {
				// For all other file types: Get blockcount, add to total.
				blkcnt_tot += sb.st_blocks;
			}
			free(path);
			path = NULL;
		}
		closedir(dir);
		

	}
	
	pthread_mutex_lock(&data->blkcnt_mut[task->job_id]);
	data->blkcnt_jobs[task->job_id] += blkcnt_tot;
	pthread_mutex_unlock(&data->blkcnt_mut[task->job_id]);

	return 0;
}

/**
 * concatenate_paths() - Given a base path, add a filename. 
 *
 * @param basepath	String with the base path name.
 * @param filename	Name of file to concatenate with base path.
 * @return			A new, dynamically allocated string with the combined path.
 */
char *concatenate_paths(char *basepath, char *filename) 
{ 
	int len_basepath, len_filename;
	char *path;
	
	// Get lengths of strings.
	len_basepath = strlen(basepath);
	len_filename = strlen(filename);

	// Allocate memory for new string.
	path = malloc((len_basepath + len_filename + 2) * sizeof(char)); 
	if (path == NULL) {
		perror("malloc");
		return NULL;
	}

	// Build the full path of the file.
	strcpy(path, basepath);
	strcat(path, "/");
	strcat(path, filename);

	return path;
}




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
		
		
		//tot += blockcount_dir(filename);
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
