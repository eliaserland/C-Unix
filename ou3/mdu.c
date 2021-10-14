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
#include <errno.h>
#include <pthread.h>
#include "list.h"

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct task_item {
	char *path; // Base path of the task.
	int job_id; // Associates the task with a specific job.
} task_item;

typedef struct public_data {
	list *queue;           // Pointer to the task queue.
	int num_jobs;          // The total number of separate jobs.
	int num_threads;       // Number of threads available.
	char **name_jobs;      // Array with length num_jobs, containing name of each job.
	blkcnt_t *blkcnt_jobs; // Array with length num_jobs, containing blockcount for each job.
	pthread_mutex_t queue_mut;   // Mutex for queue access.
	pthread_mutex_t *blkcnt_mut; // Mutexes for blockcount array access.
	pthread_cond_t cond;         // Condition variable for the queue.
} public_data;

void print_usage(char *program_name); 
public_data *init_public_data(int num_jobs, int num_threads);
task_item *create_task(char *path, int job_id);
int set_job_name(public_data *data, char *jobname, int job_id); 
blkcnt_t get_blockcount(struct stat *s); 
bool is_directory(struct stat *s); 
void add_blockcount(public_data *data, int job_id, blkcnt_t count);
char *concatenate_paths(char *basepath, char *filename);
int run_task(public_data *data, task_item *task);
void *thread_worker(void *args);
void print_results(public_data *data); 
void kill_task(task_item *task);
void kill_public_data(public_data *data); 


int main (int argc, char *argv[])
{
	// Parse program options. 
	int opt, num_jobs, s, num_threads = 1;
	while ((opt = getopt(argc, argv, "j:")) != -1) {
		switch(opt) {
		case 'j':
			num_threads = atoi(optarg);
			if (num_threads < 1) {
				fprintf(stderr, "Warning: NUM_THREADS must be "
				        "greater than or equal to 1.\n");
				exit(EXIT_FAILURE);
			}
			break;
		default:
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
		}

	}

	/* For the remaining program arguments: each specified argument 
	   (file/folder name) is a job to for the program to execute. */
	num_jobs = argc - optind;
	if (num_jobs == 0) {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Create a data structure to hold all public data, i.e. all common 
	   variables, mutexes and condition variables needed. */
	public_data *data = init_public_data(num_jobs, num_threads);
	if (data == NULL) {
		fprintf(stderr, "Couldn't initialize public data structure.\n");
		exit(EXIT_FAILURE);
	}

	// Parse each of the program arguments/jobs into initial tasks.
	task_item *task;
	for (int i = 0; i < num_jobs; i++) {
		// Create a new task item.
		task = create_task(argv[i+optind], i); 
		if (task == NULL) {
			continue;
		}

		// Let the path of the initial task be the name for the job.
		if (set_job_name(data, task->path, i)) {
			kill_task(task);
			continue; // Do something else?
		}

		// Append the task to the pulic task queue.
		if (list_append(data->queue, task)) {
			kill_task(task);
			set_job_name(data, NULL, i);
			continue;
		}
	}
	if (list_is_empty(data->queue)) {
		kill_public_data(data);
		exit(EXIT_FAILURE);
	}

	
	// Declare the thread pool.
	pthread_t *tid = calloc(num_threads-1, sizeof(*tid));
	if (tid == NULL) {
		perror("calloc");
		kill_public_data(data);
		exit(EXIT_FAILURE);
	}
	
	// Create all worker threads.
	for (int i = 0; i < num_threads-1; i++) {
		s = pthread_create(&tid[i], NULL, &thread_worker, data);
		if (s != 0) {
			handle_error_en(s, "pthread_create");
		}
	}
	
	// Start a worker in the main thread.
	thread_worker(data); // NEED TO CHECK RETURN 

	// Join all threads.
	for (int i = 0; i < num_threads-1; i++) {
		s = pthread_join(tid[i], NULL); // NEED TO CHECK RETURN 
		if (s != 0) {
			handle_error_en(s, "pthread_join");
		}
	}

	// Print results
	print_results(data);

	// Clean up all resources.
	kill_public_data(data);
	free(tid);
		
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
}

/**
 * init_public_data() - Initialize and allocate the public data structure.
 *
 * @param num_jobs	The number of jobs to execute.
 * @return		Pointer to the data structure, NULL on error.
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
	if (jobname == NULL) {
		free(data->name_jobs[job_id]);
		data->name_jobs[job_id] = NULL;
		return 0;
	}
	if ((new_name = strdup(jobname)) == NULL) {
		perror("strdup");
		data->name_jobs[job_id] = NULL;
		return 1;
	}
	data->name_jobs[job_id] = new_name;
	return 0;
}

/**
 * get_blockcount() - Wrapper to get the 512B blockcount for a stat struct. 
 *
 * @param s		Pointer to stat struct for a given file.
 * @return		The number of 512B blocks allocated on disk.
 */
blkcnt_t get_blockcount(struct stat *s) 
{
	return s->st_blocks;
}

/**
 * is_directory() - Wrapper to check if a stat struct is a directory. 
 *
 * @param s		Pointer to stat struct for a given file.
 * @return		True if the file is a directory.
 */
bool is_directory(struct stat *s) 
{
	return S_ISDIR(s->st_mode);
}

/**
 * add_blockcount() - Add blockcount to the public data structure, for a specific job. 
 *
 * @param data		Pointer to the structure with public data.
 * @param job_id	Identfier of the job.
 * @param count 	Blockcount to add for the given job id.
 */
void add_blockcount(public_data *data, int job_id, blkcnt_t count) 
{
	data->blkcnt_jobs[job_id] += count;
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
 * run_task() - Execute one task item. 
 *
 * @param data	Pointer to the structure with public data.
 * @param task	Pointer to the task data structure.
 * @return	0 if the task is successfully completed, 1 on error.
 */
int run_task(public_data *data, task_item *task) 
{
	int ret;
	char *basepath, *filename, *path;
	task_item *new_task;
	struct stat s;
	struct dirent *dir_ptr;
	DIR *dir;
	blkcnt_t blkcnt_tot = 0; // Total 512B block count for the task.

	// Root file of the task.
	basepath = task->path;

	// Get information on the file
	if (lstat(basepath, &s) < 0) {
		perror(basepath);
		return 1;
	}

	// Get blockcount of the root file.
	blkcnt_tot += get_blockcount(&s);

	/* If the root file is a directory: Open and get the blocksizes of 
	   all non-directories and add to the total, while marking each 
	   subdirectory as new tasks and appending to the task queue. */
	if (is_directory(&s)) {
		// Open the directory.
		if ((dir = opendir(basepath)) == NULL) { 
			perror(basepath);
		} else {
			// For each file in the directory:
			while ((dir_ptr = readdir(dir)) != NULL) {
				// Get the filename.
				filename = dir_ptr->d_name;

				// Filter out the always present directories "." and ".."
				if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
					continue;
				}

				// Concatenate base path and filename.
				path = concatenate_paths(basepath, filename);
				if (path == NULL) {
					continue;
				}

				// Get information of the file.
				if (lstat(path, &s) < 0) {
					perror(path); 
					free(path);
					continue;
				}

				/* Non-directory files: Add blockcount to total.
				   Directory files: Create new task, add to queue. */
				if (!is_directory(&s)) {
					blkcnt_tot += get_blockcount(&s);
				} else {
					// Create a new task item. 
					new_task = create_task(path, task->job_id);
					if (new_task == NULL) {
						continue;
					}

					// Append new task to the task queue.
					pthread_mutex_lock(&data->queue_mut);
					if (list_append(data->queue, new_task)) {
						kill_task(new_task);
						continue;
					}
					pthread_mutex_unlock(&data->queue_mut);

					// Signal condition variable.
					ret = pthread_cond_signal(&data->cond); 
					if (ret != 0) {
						handle_error_en(ret, "pthread_cond_signal");
					}
				}
				free(path);
			}
			closedir(dir);
		}
	}
	
	// Add total blockcount of the task, to the public blockcount of the job
	pthread_mutex_lock(&data->blkcnt_mut[task->job_id]);
	add_blockcount(data, task->job_id, blkcnt_tot);
	pthread_mutex_unlock(&data->blkcnt_mut[task->job_id]);

	return 0;
}

/**
 * thread_worker() - Main worker function for each thread. Until all tasks are 
 *                   completed and none more can be created, tasks are dequeued 
 *                   one by one from the the task queue and carried out.
 *
 * @param args	Pointer to the structure with public data.
 * @return	0 if all tasks are successfully completed and none more 
 *              can be created.
 */
void *thread_worker(void *args) 
{
	// Typecast args
	public_data *data = (public_data *) args;
	static int num_waiting_threads = 0; 
	static bool work_finished = false;
	int s;

	while (1) {
		pthread_mutex_lock(&data->queue_mut);
		while (list_is_empty(data->queue)) {
			if (num_waiting_threads == data->num_threads-1) {
				// This is the last working (non-waiting) thread
				work_finished = true;
				s = pthread_cond_broadcast(&data->cond);
				if (s != 0) {
					handle_error_en(s, "pthread_cond_broadcast");
				}
				pthread_mutex_unlock(&data->queue_mut);
				return 0;
			} else if (work_finished) {
				pthread_mutex_unlock(&data->queue_mut);
				return 0;
			} else {
				num_waiting_threads++;
				s = pthread_cond_wait(&data->cond, &data->queue_mut);
				if (s != 0) {
					handle_error_en(s, "pthread_cond_wait");
				}
				num_waiting_threads--;
			}
		}
		task_item *task = list_pop(data->queue);
		pthread_mutex_unlock(&data->queue_mut);
		run_task(data, task); // CHECK RETURN
		kill_task(task);
	}
}

/**
 * print_results() - Print the blockcount for each job to the terminal.
 *
 * @param data	Pointer to the structure with public data.
 */
void print_results(public_data *data) 
{
	for (int i = 0; i < data->num_jobs; i++) {
		if (data->name_jobs[i] != NULL) {
			printf("%-7ld %s\n", data->blkcnt_jobs[i], 
			       data->name_jobs[i]);
		}
	}
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
