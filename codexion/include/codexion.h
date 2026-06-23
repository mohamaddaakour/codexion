#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define FIFO 0
#define EDF 1

typedef struct s_coder t_coder;
typedef struct s_sim t_sim;
typedef struct s_dongle t_dongle;
typedef struct s_request t_request;
typedef struct s_heap t_heap;

// this is a structure for each coder
typedef struct s_coder {
	int coder_id;

	// counter for how many compiles each coder do
	int compile_count;

	pthread_t coder_thread;

	pthread_mutex_t last_compile_mutex;

	pthread_mutex_t compile_count_mutex;

	long long last_compile_start;

	t_dongle *left;
	t_dongle *right;

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

	int scheduler;

	long long global_sequence;

	long long dongle_cooldown;

	pthread_mutex_t stop_mutex;
	pthread_mutex_t print_mutex;
	pthread_mutex_t seq_mutex;

	// an array of coders
	t_coder *coders;

	t_dongle *dongles;

	pthread_t monitor;
} t_sim;

typedef struct s_dongle {
	pthread_mutex_t dongle_mutex;

	// condition variable
	pthread_cond_t cond;

	long long available_at;
	t_heap queue;
} t_dongle;

typedef struct s_request {
	int coder_id;

	// it is a number to specify how requested the dongle first
	long long sequence;

	long long deadline;
} t_request;

typedef struct s_heap {
	// this is array of requests
	t_request *nodes;

	// this is the number of requests in the heap
	int size;

	// this is the number of coders
	int capacity;
} t_heap;

long long get_time_in_ms();
void precise_sleep(long long ms);
int get_stop(t_sim *sim);
void set_stop(t_sim *sim, int stop_value);
void print_status(t_coder *coder, char *message);
void *coder_routine(void *arg);
void *monitor_routine(void *arg);
void take_dongles(t_coder *coder);
void release_dongles(t_coder *coder);
int init_heap(t_heap *heap, int capacity);
int compare_req(t_request a, t_request b, int scheduler);
void heap_push(t_heap *heap, t_request req, int scheduler);
void heap_pop(t_heap *heap, int scheduler);
t_request heap_peek(t_heap *heap);
void heap_remove(t_heap *heap, int coder_id, int scheduler);

#endif
