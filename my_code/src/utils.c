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