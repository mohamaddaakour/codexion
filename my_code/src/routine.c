#include "../include/codexion.h"

void *coder_routine(void *arg) {
    t_coder *coder = (t_coder *)arg;

    while (!get_stop(coder->sim)) {

        // we put the last time this coder compiles
        pthread_mutex_lock(&coder->last_compile_mutex);
        coder->last_compile_start = get_time_ms();
        pthread_mutex_unlock(&coder->last_compile_mutex);

        // to print the status message
        print_status("%lld %d %s\n", get_time_ms(), coder->coder_id, "is compiling");

        // get sleep in the compilation time
        precise_sleep(coder->sim->time_to_compile);

        // to increment the compile count
        pthread_mutex_lock(&coder->compile_count_mutex);
        coder->compile_count++;
        pthread_mutex_unlock(&coder->compile_count_mutex);

        
        if (get_stop(c->sim))
            break;
        print_status("%lld %d %s\n", get_time_ms(), coder->coder_id, "is debugging");
        precise_sleep(coder->sim->time_to_debug);

        if (get_stop(c->sim))
            break;
        print_status("%lld %d %s\n", get_time_ms(), coder->coder_id, "is refactoring");
        precise_sleep(coder->sim->time_to_refactor);

    }

    return NULL;
}

// if all_done = 0 means all coders are done
void *monitor_routine(void *arg) {
    t_sim *sim = (t_sim *)arg;

    while (!get_stop(sim)) {
        int all_done = 1;

        pthread_mutex_lock(&coder->last_compile_mutex);
        long long last_compile_time = coder->last_compile_start;
        pthread_mutex_unlock(&coder->last_compile_mutex);

        if ((get_time_ms() - last_compile_time) >= sim.time_to_burnout) {
            print_status(sim, "has burned out");
            all_done = 1;
        }

        if (sim->required_compiles == -1 || required_compiles <= sim->coders->compile_count) {
            all_done = 0;
        }
    }

    return NULL;
}