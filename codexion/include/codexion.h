#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct s_coder t_coder;
typedef struct s_sim t_sim;

// this is a structure for each coder
typedef struct s_coder {
	int coder_id;

	// counter for how many compiles each coder do
	int compile_count;

	pthread_t coder_thread;

	pthread_mutex_t last_compile_mutex;

	pthread_mutex_t compile_count_mutex;

	long long last_compile_start;

	t_sim *sim;

} t_coder;

// this is a simulation structure
typedef struct s_sim {
	int number_of_coders;

	long long time_to_compile;
	long long time_to_debug;
	long long time_to_refactor;
	long long time_to_burnout;

	// how many compiles each coder have to do to finish
	int number_of_compiles_required;

	long long start_time;

	// this will be 1 if the simulation will stop or 0 otherwise
	int stop;

	pthread_mutex_t stop_mutex;
	pthread_mutex_t print_mutex;

	// an array of coders
	t_coder *coders;

	pthread_t monitor;
} t_sim;

long long get_time_in_ms();
void precise_sleep(long long ms);
int get_stop(t_sim *sim);
void set_stop(t_sim *sim, int stop_value);
void print_status(t_coder *coder, char *message);
void *coder_routine(void *arg);
void *monitor_routine(void *arg);

# endif
