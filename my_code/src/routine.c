#include "../include/codexion.h"

void *coder_routine(void *arg) {
    t_coder *coder = (t_coder *)arg;

    while (!get_stop(coder->sim)) {

        // to take dongles
        take_dongles(coder);

        if (get_stop(coder->sim))
            break;

        // we put the last time this coder compiles
        pthread_mutex_lock(&coder->last_compile_mutex);
        coder->last_compile_start = get_time_ms();
        pthread_mutex_unlock(&coder->last_compile_mutex);

        // to print the status message
        print_status(coder, "is compiling");

        // get sleep in the compilation time
        precise_sleep(coder->sim->time_to_compile);

        // to increment the compile count
        pthread_mutex_lock(&coder->compile_count_mutex);
        coder->compile_count++;
        pthread_mutex_unlock(&coder->compile_count_mutex);

        // after finishing compilation we release the dongles
        release_dongles(coder);


        if (get_stop(coder->sim))
            break;
        print_status(coder, "is debugging");
        precise_sleep(coder->sim->time_to_debug);

        if (get_stop(coder->sim))
            break;
        print_status(coder, "is refactoring");
        precise_sleep(coder->sim->time_to_refactor);

        usleep(1000);
    }

    return NULL;
}

// if all_done = 1 means all coders are done
// required_compiles = -1 the coders will compile infinetly until someone burns out
void *monitor_routine(void *arg) {
    t_sim *sim = (t_sim *)arg;

    while (!get_stop(sim)) {
        int all_done = 1;

        int i = 0;

        // each time we are checking for all coders
        while (i < sim->number_of_coders) {
            pthread_mutex_lock(&sim->coders[i].last_compile_mutex);
            long long last_compile_time = sim->coders[i].last_compile_start;
            pthread_mutex_unlock(&sim->coders[i].last_compile_mutex);

            // if a coder exceeded the burnout time without compiling
            if ((get_time_ms() - last_compile_time) >= sim->time_to_burnout) {
                print_status(&sim->coders[i], "has burned out");
                set_stop(sim, 1);
                return NULL;
            }

            // here we check if the compilation hasn't finished yet
            pthread_mutex_lock(&sim->coders[i].compile_count_mutex);
			if (sim->required_compiles != -1 && sim->coders[i].compile_count < sim->required_compiles) {
				all_done = 0;
			}
			pthread_mutex_unlock(&sim->coders[i].compile_count_mutex);

            i++;
        }

        if (sim->required_compiles != -1 && all_done) {
			set_stop(sim, 1);
			return NULL;
		}

        usleep(1000);
    }

    return NULL;
}
