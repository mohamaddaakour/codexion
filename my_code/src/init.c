#include "../include/codexion.h"

int init_simulation(t_sim *sim)
{
    if (pthread_mutex_init(&sim->stop_mutex, NULL) || pthread_mutex_init(&sim->print_mutex, NULL) || pthread_mutex_init(&sim->seq_mutex, NULL)) return (1);
    sim->stop = 0; sim->global_sequence = 0;
    sim->dongles = malloc(sizeof(t_dongle) * sim->num_coders);
    sim->coders = malloc(sizeof(t_coder) * sim->num_coders);
    if (!sim->dongles || !sim->coders) return (1);
    for (int i = 0; i < sim->num_coders; i++) {
        pthread_mutex_init(&sim->dongles[i].mutex, NULL);
        pthread_cond_init(&sim->dongles[i].cond, NULL);
        sim->dongles[i].available_at = 0;
        if (init_heap(&sim->dongles[i].queue, sim->num_coders)) return (1);
    }
    sim->start_time = get_time_ms();
    for (int i = 0; i < sim->num_coders; i++) {
        sim->coders[i].id = i + 1;
        sim->coders[i].compile_count = 0;
        sim->coders[i].sim = sim;
        sim->coders[i].left = &sim->dongles[i];
        sim->coders[i].right = &sim->dongles[(i + 1) % sim->num_coders];
        sim->coders[i].last_compile_start = sim->start_time;
        pthread_mutex_init(&sim->coders[i].last_compile_mutex, NULL);
    }
    return (0);
}

int parse_args(int argc, char **argv, t_sim *sim)
{
    if (argc != 9) return (1);
    sim->num_coders = atoi(argv[1]);
    sim->time_to_burnout = atoi(argv[2]);
    sim->time_to_compile = atoi(argv[3]);
    sim->time_to_debug = atoi(argv[4]);
    sim->time_to_refactor = atoi(argv[5]);
    sim->required_compiles = atoi(argv[6]);
    sim->dongle_cooldown = atoi(argv[7]);
    if (sim->num_coders <= 0 || sim->time_to_burnout <= 0 || sim->time_to_compile < 0 || sim->time_to_debug < 0 || sim->time_to_refactor < 0 || sim->dongle_cooldown < 0) return (1);
    if (strcmp(argv[8], "fifo") == 0) sim->scheduler = FIFO;
    else if (strcmp(argv[8], "edf") == 0) sim->scheduler = EDF;
    else return (1);
    return (0);
}