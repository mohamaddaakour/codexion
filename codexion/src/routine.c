#include "../include/codexion.h"

// this function will be applied for every coder thread when we create it
// always when we have to create a thread the return type will be a generic pointer
// and the parameter also
// and the return will be NULL
void *coder_routine(void *arg) {

	// casting
	t_coder *coder = (t_coder *)arg;

	// this loop will stay for all coders until the simulation ends
	while (!get_stop(coder->sim)) {
		take_dongles(coder);

		pthread_mutex_lock(&coder->last_compile_mutex);
		coder->last_compile_start = get_time_in_ms();
		pthread_mutex_unlock(&coder->last_compile_mutex);

		print_status(coder, "is compiling");
		precise_sleep(coder->sim->time_to_compile);

		pthread_mutex_lock(&coder->compile_count_mutex);
		coder->compile_count++;
		pthread_mutex_unlock(&coder->compile_count_mutex);

		release_dongles(coder);

		if (get_stop(coder->sim)) {
			break;
		}

		print_status(coder, "is debugging");
		precise_sleep(coder->sim->time_to_debug);

		if (get_stop(coder->sim)) {
			break;
		}

		print_status(coder, "is refactoring");
		precise_sleep(coder->sim->time_to_refactor);

		usleep(1000);
	}

	return NULL;
}

void *monitor_routine(void *arg) {
	t_sim *sim = (t_sim*)arg;

	while (!get_stop(sim)) {
		// first we assume that all coders have finished all there required compiles
		// until we prove the oposite
		int all_done = 1;

		int i = 0;

		while (i < sim->number_of_coders) {
			pthread_mutex_lock(&sim->coders[i].last_compile_mutex);
			long long last = sim->coders[i].last_compile_start;
			pthread_mutex_unlock(&sim->coders[i].last_compile_mutex);

			// if a coder passed the time to burnout without compiling he will burn out
			// and the simulation will stop
			// we added 2ms to make the code precise
			if ((get_time_in_ms() - last) > (sim->time_to_burnout + 2)) {
				print_status(&sim->coders[i], "is burnout");
				set_stop(sim, 1);
				return NULL;
			}

			// here we check if the compliation isn't finish yet
			// if required compiles is -1 this means the coder will stay compiling until someone burns out
			pthread_mutex_lock(&sim->coders[i].compile_count_mutex);
			if (sim->number_of_compiles_required != -1 && sim->coders[i].compile_count < sim->number_of_compiles_required) {
				all_done = 0;
			}
			pthread_mutex_unlock(&sim->coders[i].compile_count_mutex);

			i++;
		}

		if (sim->number_of_compiles_required != -1 && all_done) {
			set_stop(sim, 1);
			return NULL;
		}

		// put a small sleep at the end of routine for optimization and let the locks catched perfectly
		usleep(1000);
	}
	return NULL;
}
