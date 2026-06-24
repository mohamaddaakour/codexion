#include "../include/codexion.h"

int get_stop(t_sim *sim) {
    int stop;

    // we lock to prevent 2 coders access stop in the same time
    pthread_mutex_lock(&sim->stop_mutex);
    stop = sim->stop;
    pthread_mutex_unlock(&sim->stop_mutex);

    return stop;
}

void set_stop(t_sim *sim, int value) {
    pthread_mutex_lock(&sim->stop_mutex);
    sim->stop = value;
    pthread_mutex_unlock(&sim->stop_mutex);
}

void print_status(t_coder *coder, char *message) {
    long long time;

    pthread_mutex_lock(&coder->sim->print_mutex);

    // in case of burn out the stop is equal 1 in this case the program will stop without
    // knowing we have a burn out for this case i used strcmp
    if (!get_stop(coder->sim) || strcmp(msg, "burned out") == 0) {
        time = get_time_ms() - coder->sim->start_time;
        printf("%lld %d %s\n", time, coder->coder_id, message);
    }
    pthread_mutex_unlock(&coder->sim->print_mutex);
}