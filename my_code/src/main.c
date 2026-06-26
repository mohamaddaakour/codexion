#include "../include/codexion.h"

// int main(int argc, char **argv) {

//     if (argc != 7) {
//         printf("You should give 7 arguments!");
//         return 1;
//     }

//     t_sim sim;

//     memset(&sim, 0, sizeof(t_sim));

//     // we initialize the sim
//     sim.number_of_coders = atoi(argv[1]);
//     sim.time_to_burnout = atoi(argv[2]);
//     sim.time_to_compile = atoi(argv[3]);
//     sim.time_to_debug = atoi(argv[4]);
//     sim.time_to_refactor = atoi(argv[5]);
//     sim.required_compiles = atoi(argv[6]);

//     sim.start_time = get_time_ms();

//     sim.stop = 0;

//     // we initialize the mutexes
//     pthread_mutex_init(&sim.stop_mutex, NULL);
//     pthread_mutex_init(&sim.print_mutex, NULL);

//     // we allocate a memory for the coders and dongles arrays
//     sim.coders = (t_coder *)malloc(sizeof(t_coder) * sim.number_of_coders);
//     sim.dongles = (t_dongle *)malloc(sizeof(t_dongle) * sim.number_of_coders);

//     if (!sim.coders)
//         return 1;

//     if (!sim.dongles)
//         return 1;

//     int i = 0;

//     while (i < sim.number_of_coders) {
//         pthread_mutex_init(&sim.dongles[i].dongle_mutex, NULL);
//         i++;
//     }

//     i = 0;

//     // we initialize the coders
//     while (i < sim.number_of_coders) {
//         sim.coders[i].coder_id = i + 1;
//         sim.coders[i].last_compile_start = sim.start_time;
//         sim.coders[i].compile_count = 0;

//         sim.coders[i].sim = &sim;

//         sim.coders[i].left = &sim.dongles[i];
//         sim.coders[i].right = &sim.dongles[(i + 1) % sim.number_of_coders];

//         // we create the mutexes for each coder
//         pthread_mutex_init(&sim.coders[i].last_compile_mutex, NULL);
//         pthread_mutex_init(&sim.coders[i].compile_count_mutex, NULL);

//         i++;
//     }

//     i = 0;

//     while (i < sim.number_of_coders) {
//         pthread_create(&sim.coders[i].coder_thread, NULL, coder_routine, &sim.coders[i]);
//         i++;
//     }

//     // we create the thread for the monitor
//     pthread_create(&sim.monitor_thread, NULL, monitor_routine, &sim);

//     // we join the threads
//     pthread_join(sim.monitor_thread, NULL);

//     i = 0;

//     while (i < sim.number_of_coders) {
//         pthread_join(sim.coders[i].coder_thread, NULL);
//         i++;
//     }

//     i = 0;

//     // we destroyed the mutexes for coders
//     while (i < sim.number_of_coders) {
//         pthread_mutex_destroy(&sim.coders[i].last_compile_mutex);
//         pthread_mutex_destroy(&sim.coders[i].compile_count_mutex);
//         i++;
//     }

//     i = 0;

//     while (i < sim.number_of_coders) {
//         pthread_mutex_destroy(&sim.dongles[i].dongle_mutex);
//         i++;
//     }

//     // we destroyed the mutexes for sim
//     pthread_mutex_destroy(&sim.stop_mutex);
//     pthread_mutex_destroy(&sim.print_mutex);

//     // we free the allocated memory for the coders array
//     free(sim.coders);
//     free(sim.dongles);

//     return 0;
// }

int main(int argc, char **argv)
{
    t_sim sim;
    memset(&sim, 0, sizeof(t_sim));
    if (parse_args(argc, argv, &sim)) { printf("Invalid args\n"); return (1); }
    if (init_simulation(&sim)) { printf("Init failed\n"); destroy_sim(&sim); return (1); }
    for (int i = 0; i < sim.num_coders; i++) pthread_create(&sim.coders[i].thread, NULL, coder_routine, &sim.coders[i]);
    pthread_create(&sim.monitor, NULL, monitor_routine, &sim);
    pthread_join(sim.monitor, NULL);
    for (int i = 0; i < sim.num_coders; i++) pthread_join(sim.coders[i].thread, NULL);
    destroy_sim(&sim);
    return (0);
}