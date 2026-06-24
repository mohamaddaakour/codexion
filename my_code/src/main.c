#include "../include/codexion.h"

int main(int argc, char **argv) {

    if (argc != 7) {
        printf("You should give 7 arguments!");
        return 1;
    }

    t_sim sim;

    memset(&sim, 0, sizeof(t_sim));

    // we initialize the sim
    sim.number_of_coders = atoi(argv[1]);
    sim.time_to_burnout = atoi(argv[2]);
    sim.time_to_compile = atoi(argv[3]);
    sim.time_to_refactor = atoi(argv[4]);
    sim.required_compiles = atoi(argv[5]);
    sim.start_time = get_time_ms();

    sim.stop = 0;

    // we initialize the mutexes
    pthread_mutex_init(&sim.stop_mutex, NULL);
    pthread_mutex_init(&sim.print_mutex, NULL);

    sim.coders = (t_coder *)malloc(sizeof(t_coder) * sim.number_of_coders);

    // we create the thread for the monitor
    pthread_create(&sim.monitor_thread, NULL, monitor_routine, &sim);

    int i = 1;

    // we initialize the coders
    while (i <= sim.number_of_coders) {
        sim.coders[i].coder_id = i;
        sim.coders[i].last_compile_start = sim.start_time;
        sim.coders[i].compile_count = 0;

        // we create a thread for each coder
        pthread_create(&sim.coders[i].coder_thread, NULL, coder_routine, &sim.coders[i]);

        pthread_mutex_init(&sim.coders[i].last_compile_mutex, NULL);
        pthread_mutex_init(&sim.coders[i].compile_count_mutex, NULL);

        sim.coders->sim = &sim;

        i++;
    }

    // we join the threads
    pthread_join(sim.monitor_thread, NULL);

    i = 1;

    while (i <= sim.number_of_coders) {
        pthread_join(sim.coders[i].coder_thread, NULL);
    }

    i = 1;

    // we destroyed the mutexes for coders
    while (i <= sim.number_of_coders) {
        pthread_mutex_destroy(&sim.coders[i].last_compile_mutex);
        pthread_mutex_destroy(&sim.coders[i].compile_count_mutex);
    }

    // we destroyed the mutexes for sim
    pthread_mutex_destroy(&sim.stop_mutex);
    pthread_mutex_destroy(&sim.print_mutex);

    // we free the allocated memory for the coders array
    free(sim.coders);

    return 0;
}