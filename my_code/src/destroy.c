#include "../include/codexion.h"

void    destroy_sim(t_sim *sim)
{
    if (!sim) return ;
    if (sim->dongles) {
        for (int i = 0; i < sim->num_coders; i++) {
            pthread_mutex_destroy(&sim->dongles[i].mutex);
            pthread_cond_destroy(&sim->dongles[i].cond);
            free(sim->dongles[i].queue.nodes);
        }
        free(sim->dongles);
    }
    if (sim->coders) {
        for (int i = 0; i < sim->num_coders; i++)
            pthread_mutex_destroy(&sim->coders[i].last_compile_mutex);
        free(sim->coders);
    }
    pthread_mutex_destroy(&sim->stop_mutex);
    pthread_mutex_destroy(&sim->print_mutex);
    pthread_mutex_destroy(&sim->seq_mutex);
}