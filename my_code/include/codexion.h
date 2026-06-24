// this is include guards, here we just declaring not implementing
// this will prevent from implementing something twice
#ifndef CODEXION_H
#define CODEXION_H

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

typedef struct s_coder {
    int coder_id;
    long long last_compile_start;
    int compile_count;
    pthread_t coder_thread;
    pthread_mutex_t last_compile_mutex;
    pthread_mutex_t compile_count_mutex;

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

    pthread_t monitor_thread;
} t_sim;

// declaring functions

long long get_time_ms();
void precise_sleep(long long ms);
int get_stop(t_sim *sim);
void set_stop(t_sim *sim, int value);
void print_status(t_coder *coder, char *message);
void *coder_routine(void *arg);
void *monitor_routine(void *arg);


#endif