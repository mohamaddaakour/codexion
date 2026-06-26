// this is include guards, here we just declaring not implementing
// this will prevent from implementing something twice
#ifndef CODEXION_H
#define CODEXION_H

#define FIFO 0
#define EDF 1

// declaring libraries

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

// declaring structures

typedef struct s_coder t_coder;
typedef struct s_sim t_sim;
typedef struct s_dongle t_dongle;
typedef struct s_request t_request;
typedef struct s_heap t_heap;

typedef struct s_coder {
    int coder_id;
    long long last_compile_start;
    int compile_count;
    pthread_t coder_thread;
    pthread_mutex_t last_compile_mutex;
    pthread_mutex_t compile_count_mutex;

    t_dongle *left;
    t_dongle *right;

    t_sim *sim;
} t_coder;

typedef struct s_sim {
    int number_of_coders;

    long long time_to_burnout;
    long long time_to_compile;
    long long time_to_debug;
    long long time_to_refactor;

    int required_compiles;
    long long start_time;
    int stop;

    pthread_mutex_t stop_mutex;
    pthread_mutex_t print_mutex;

    t_coder *coders;

    t_dongle *dongles;

    pthread_t monitor_thread;

    long long dongle_cooldown;
    int scheduler;
    long long global_sequence;

    pthread_mutex_t seq_mutex;
} t_sim;

typedef struct s_dongle {
    pthread_mutex_t dongle_mutex;
    pthread_cond_t cond;
    long long available_at;
    t_heap queue;
} t_dongle;

typedef struct s_request {
    int         coder_id;
    long long   sequence;
    long long   deadline;
} t_request;

typedef struct s_heap {
    t_request   *nodes;
    int         size;
    int         capacity;
} t_heap;

// declaring functions

long long get_time_ms();
void precise_sleep(long long ms);
int get_stop(t_sim *sim);
void set_stop(t_sim *sim, int value);
void print_status(t_coder *coder, char *message);
void *coder_routine(void *arg);
void *monitor_routine(void *arg);
void release_dongles(t_coder *coder);
void take_dongles(t_coder *coder);
int init_heap(t_heap *heap, int capacity);
int compare_req(t_request a, t_request b, int scheduler);
void heap_push(t_heap *heap, t_request req, int scheduler);
void heap_pop(t_heap *heap, int scheduler);
t_request heap_peek(t_heap *heap);
void heap_remove(t_heap *heap, int coder_id, int scheduler);


#endif
