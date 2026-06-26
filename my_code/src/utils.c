#include "../include/codexion.h"

long long get_time_ms() {
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void precise_sleep(long long ms) {
    long long start = get_time_ms();

    while ((get_time_ms() - start) <= ms)
        usleep(100);
}

long long get_next_sequence(t_sim *sim) {
    long long s;
    
    pthread_mutex_lock(&sim->seq_mutex);
    s = sim->global_sequence++;
    pthread_mutex_unlock(&sim->seq_mutex);
    
    return s;
}