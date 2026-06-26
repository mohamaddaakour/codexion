#include "../include/codexion.h"

// to let the coder take the dongles
void take_dongles(t_coder *coder) {
	// if we have only one coder this mean we have only on dongle
	// so the compilation can't be done
	if (coder->sim->number_of_coders == 1) {
		pthread_mutex_lock(&coder->left->dongle_mutex);
		print_status(coder, "has taken a dongle");
		precise_sleep(coder->sim->time_to_burnout);

		return;
	}

	// to prevent the deadlock we take the dongle having the lowest memory address
	// first
	if (coder->left < coder->right) {
		pthread_mutex_lock(&coder->left->dongle_mutex);
		pthread_mutex_lock(&coder->right->dongle_mutex);
	} else {
		pthread_mutex_lock(&coder->right->dongle_mutex);
		pthread_mutex_lock(&coder->left->dongle_mutex);
	}
	print_status(coder, "has taken a dongle");
	print_status(coder, "has taken a dongle");
}

void release_dongles(t_coder *coder) {
	if (coder->sim->number_of_coders == 1) {
		pthread_mutex_unlock(&coder->left->dongle_mutex);

		return;
	}

	pthread_mutex_unlock(&coder->left->dongle_mutex);
	pthread_mutex_unlock(&coder->right->dongle_mutex);
}

void    acquire_dongle(t_dongle *dongle, t_coder *coder) {
    t_request req;

    pthread_mutex_lock(&dongle->dongle_mutex);
    req.coder_id = coder->coder_id;
    req.sequence = get_next_sequence(coder->sim);

    pthread_mutex_lock(&coder->last_compile_mutex);
    req.deadline = coder->last_compile_start + coder->sim->time_to_burnout;
    pthread_mutex_unlock(&c->last_compile_mutex);

    heap_push(&dongle->queue, req, coder->sim->scheduler);

    while (!get_stop(coder->sim)) {
        long long now = get_time_ms();

        t_request top = heap_peek(&d->queue);

        if (top.coder_id == c->id && now >= d->available_at)
            break;

        if (top.coder_id == coder->coder_id && dongle->available_at > now) {
            struct timespec ts;

            ts.tv_sec = dongle->available_at / 1000;
            ts.tv_nsec = (dongle->available_at % 1000) * 1000000;

            pthread_cond_timedwait(&d->cond, &d->mutex, &ts);
        }

        else
            pthread_cond_wait(&d->cond, &d->mutex);
    }

    if (!get_stop(c->sim))
        heap_pop(&d->queue, c->sim->scheduler);

    else
        heap_remove(&d->queue, c->id, c->sim->scheduler);
    pthread_mutex_unlock(&d->mutex);
}

void    release_dongle(t_dongle *dongle, t_sim *sim) {
    pthread_mutex_lock(&dongle->dongle_mutex);
    dongle->available_at = get_time_ms() + sim->dongle_cooldown;
    pthread_cond_broadcast(&dongle->cond);
    pthread_mutex_unlock(&dongle->dongle_mutex);
}