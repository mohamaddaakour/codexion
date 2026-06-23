#include "../include/codexion.h"

// to get the stop value
int get_stop(t_sim *sim) {
	pthread_mutex_lock(&sim->stop_mutex);

	int stop = sim->stop;

	pthread_mutex_unlock(&sim->stop_mutex);

	return stop;
}

// to modify the stop value
void set_stop(t_sim *sim, int stop_value) {
	pthread_mutex_lock(&sim->stop_mutex);

	sim->stop = stop_value;

	pthread_mutex_unlock(&sim->stop_mutex);
}

// to print the process
void print_status(t_coder *coder, char *message) {

	long long time;

	pthread_mutex_lock(&coder->sim->print_mutex);

	if (!get_stop(coder->sim) || strcmp(message, "is burnout") == 0) {
		time = get_time_in_ms() - coder->sim->start_time;

		printf("%lld %d %s\n", time, coder->coder_id, message);
	}

	pthread_mutex_unlock(&coder->sim->print_mutex);
}

long long get_next_sequence(t_sim *sim) {
	long long s;

	pthread_mutex_lock(&sim->seq_mutex);
	s = sim->global_sequence++;
	pthread_mutex_unlock(&sim->seq_mutex);

	return s;
}
