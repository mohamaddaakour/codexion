#include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
#include <pthread.h>

// Forward declarations
typedef struct s_sim t_sim;

typedef struct s_sim {
    int             stop;
    pthread_mutex_t stop_mutex;
} t_sim;

// The thread-safe getter function we are testing
int get_stop(t_sim *sim) {
    int stop;
    pthread_mutex_lock(&sim->stop_mutex);
    stop = sim->stop;
    pthread_mutex_unlock(&sim->stop_mutex);
    return stop;
}

// A background worker thread that continuously checks the stop flag
void *worker_routine(void *arg) {
    t_sim *sim = (t_sim *)arg;

    printf("[Worker] Thread started. Checking get_stop()...\n");

    // Loop safely using the getter function
    while (get_stop(sim) == 0) {
        printf("[Worker] get_stop() returned 0. Still running...\n");
        sleep(1); // Wait 1 second before checking again
    }

    printf("[Worker] Alert! get_stop() returned 1! Thread shutting down cleanly.\n");
    return NULL;
}

int main(void) {
    t_sim       sim;
    pthread_t   worker_thread;

    // 1. Initialize the simulation structure and the mutex
    sim.stop = 0;
    if (pthread_mutex_init(&sim.stop_mutex, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    printf("[Main] Creating background worker thread...\n");

    // 2. Launch the worker thread
    if (pthread_create(&worker_thread, NULL, worker_routine, &sim) != 0) {
        printf("Failed to create thread\n");
        pthread_mutex_destroy(&sim.stop_mutex);
        return 1;
    }

    // 3. Let the main thread sleep for 3 seconds while the worker loops
    printf("[Main] Sleeping for 3 seconds...\n");
    sleep(3);

    // 4. Change the stop flag safely by locking the mutex
    printf("[Main] Waking up. Locking mutex to change stop flag to 1...\n");
    pthread_mutex_lock(&sim.stop_mutex);
    sim.stop = 1;
    pthread_mutex_unlock(&sim.stop_mutex);
    printf("[Main] Mutex unlocked. stop flag is now 1.\n");

    // 5. Wait for the worker thread to see the change and exit
    pthread_join(worker_thread, NULL);
    printf("[Main] Worker thread successfully joined. Cleaning up.\n");

    // 6. Clean up resources
    pthread_mutex_destroy(&sim.stop_mutex);
    printf("[Main] Test finished successfully!\n");

    return 0;
}
