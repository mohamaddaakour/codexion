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
