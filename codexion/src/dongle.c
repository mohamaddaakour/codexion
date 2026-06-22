#include "../include/codexion.h"

void take_dongles(t_coder *coder) {
    // if we have only one coder this means we have only one dongle
    // so the coder will take the left dongle and burnout without compiling
    if (coder->sim->number_of_coders == 1) {
        pthread_mutex_lock(&coder->left->dongle_mutex);
        print_status(coder, "has taken a dongle");
        precise_sleep(coder->sim->time_to_burnout + 10);

        return;
    }

    if (coder->left < coder->right) {
        pthread_mutex_lock(&coder->left->dongle_mutex);
        print_status(coder, "has taken a dongle");

        pthread_mutex_lock(&coder->right->dongle_mutex);
        print_status(coder, "has taken a dongle");
    } else {
        pthread_mutex_lock(&coder->right->dongle_mutex);
        print_status(coder, "has taken a dongle");

        pthread_mutex_lock(&coder->left->dongle_mutex);
        print_status(coder, "has taken a dongle");
    }
}

void release_dongles(t_coder *coder) {
    if (coder->sim->number_of_coders == 1) {
        pthread_mutex_unlock(&coder->left->dongle_mutex);
        return;
    }

    pthread_mutex_unlock(&coder->left->dongle_mutex);
    pthread_mutex_unlock(&coder->right->dongle_mutex);
}