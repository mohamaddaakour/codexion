#include "../include/codexion.h"

// make the coders take 2 dongles (left and right) to start compile
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

// make the coders release the dongles and put them back in the table
void release_dongles(t_coder *coder) {
    if (coder->sim->number_of_coders == 1) {
        pthread_mutex_unlock(&coder->left->dongle_mutex);
        return;
    }

    pthread_mutex_unlock(&coder->left->dongle_mutex);
    pthread_mutex_unlock(&coder->right->dongle_mutex);
}

// when the coder ask for a dongle
void    acquire_dongle(t_dongle *dongle, t_coder *coder) {
    t_request req;

    pthread_mutex_lock(&dongle->dongle_mutex);
    req.coder_id = coder->coder_id;
    req.sequence = get_next_sequence(coder->sim);

    pthread_mutex_lock(&coder->last_compile_mutex);
    req.deadline = coder->last_compile_start + coder->sim->time_to_burnout;
    pthread_mutex_unlock(&coder->last_compile_mutex);

    heap_push(&dongle->queue, req, coder->sim->scheduler);

    while (!get_stop(coder->sim)) {
        long long now = get_time_ms();
        t_request top = heap_peek(&dongle->queue);

        if (top.coder_id == coder->coder_id && now >= dongle->available_at)
            break;

        if (top.coder_id == coder->coder_id && dongle->available_at > now) {
            struct timespec ts;
            ts.tv_sec = dongle->available_at / 1000;
            ts.tv_nsec = (dongle->available_at % 1000) * 1000000;
            pthread_cond_timedwait(&dongle->cond, &dongle->dongle_mutex, &ts);
        }
        else
            pthread_cond_wait(&dongle->cond, &dongle->dongle_mutex);
    }

    if (!get_stop(coder->sim))
        heap_pop(&dongle->queue, coder->sim->scheduler);
    else
        heap_remove(&dongle->queue, coder->coder_id, coder->sim->scheduler);

    pthread_mutex_unlock(&dongle->dongle_mutex);
}
