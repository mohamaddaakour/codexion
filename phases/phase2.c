#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct s_sim    t_sim;
typedef struct s_coder  t_coder;
typedef struct s_dongle t_dongle;

// dongle structure
typedef struct s_dongle {
	// this is an object used to lock and unlock
	pthread_mutex_t mutex;
} t_dongle;

// coder struct
typedef struct s_coder {
	int id;
	int compile_count;
	long long last_compile_start;

	// to create a thread
	pthread_t thread;

	t_dongle *left;
	t_dongle *right;

	pthread_mutex_t last_compile_mutex;

	t_sim *sim;
} t_coder;

typedef struct s_sim {
	int num_coders;

	long long time_to_compile;
	long long time_to_debug;
	long long time_to_refactor;

	// this is the duration if one coder stay without compiling for this amount of time
	// he will get burned out and the program will stop
	long long time_to_burnout;

	int required_compiles;

	long long start_time;
	int stop;

	pthread_mutex_t stop_mutex;
	pthread_mutex_t print_mutex;

	t_coder *coders;
	t_dongle *dongles;

	pthread_t monitor;
} t_sim;

// this function gives the current time in milleseconds
long long get_time_ms() {
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void precise_sleep(long long ms) {
	long long start = get_time_ms();

	while ((get_time_ms() - start) < ms) {
		usleep(200);
	}
}

// we used *sim not sim because we want to change the sim
// in the main not a copy from it
// we can't change the stop for 2 different threads
// stop will be 1 if the coder burned out or finished and 0 otherwise
int get_stop(t_sim *sim) {
	int stop;

	// this will lock stop_mutex and if another thread try to execute
	// this line will be blocked
	pthread_mutex_lock(&sim -> stop_mutex);

	stop = sim -> stop;

	// we unlock it, now another thread can execute the lock
	pthread_mutex_unlock(&sim -> stop_mutex);

	return stop;
}

// to modify the stop
void set_stop(t_sim *sim, int value) {
    pthread_mutex_lock(&sim->stop_mutex);
    sim->stop = value;
    pthread_mutex_unlock(&sim->stop_mutex);
}

// if multiple threads try to print to the console in the same time, we will
// get a corrupted output for that reason we used this function
void print_status(t_coder *coder, char *msg) {
	// we locked to print the status of a specific thread only
	pthread_mutex_lock(&coder -> sim -> print_mutex);

	// if the coder haven't finished or didn't burnout
	if (!get_stop(coder -> sim) || strcmp(msg, "burned out") == 0) {
		long long time = get_time_ms() - (coder -> sim -> start_time);
		printf("%lld %d %s\n", time, coder -> id, msg);
	}
	pthread_mutex_unlock(&coder -> sim -> print_mutex);
}

// this function to take the two dongles to start compiling
void take_dongles(t_coder *c) {
	// to prevent the deadlock maybe first thread will take the left dongle
	// and second thread takes the right dongle we will get a deadlock in this case
	// to prevent this we see the address of each dongle and we begin with the lower address
    if (c->left < c->right) {
        pthread_mutex_lock(&c->left->mutex);
        print_status(c, "has taken a left dongle");
        pthread_mutex_lock(&c->right->mutex);
        print_status(c, "has taken a right dongle");
    } else {
        pthread_mutex_lock(&c->right->mutex);
        print_status(c, "has taken a right dongle");
        pthread_mutex_lock(&c->left->mutex);
        print_status(c, "has taken a left dongle");
    }
}

// to put the dongles again in the table so another thread can take them
void release_dongles(t_coder *c) {
    pthread_mutex_unlock(&c->left->mutex);
    pthread_mutex_unlock(&c->right->mutex);
}

void *coder_routine(void *arg) {
    t_coder *c = (t_coder *)arg;

    if (c->id % 2 == 0)
		usleep(1000);

    while (!get_stop(c->sim)) {
        take_dongles(c);

        pthread_mutex_lock(&c->last_compile_mutex);

		// we take the time when the coder did his first compilation
        c->last_compile_start = get_time_ms();

        pthread_mutex_unlock(&c->last_compile_mutex);

        print_status(c, "is compiling");
        precise_sleep(c->sim->time_to_compile);

        pthread_mutex_lock(&c->sim->stop_mutex);
        c->compile_count++;
        pthread_mutex_unlock(&c->sim->stop_mutex);

        release_dongles(c);

        print_status(c, "is debugging");
        precise_sleep(c->sim->time_to_debug);
        print_status(c, "is refactoring");
        precise_sleep(c->sim->time_to_refactor);
    }
    return NULL;
}

void *monitor_routine(void *arg) {
    t_sim *sim = (t_sim *)arg;

    while (!get_stop(sim)) {
        int all_done = 1;
        for (int i = 0; i < sim->num_coders; i++) {
            pthread_mutex_lock(&sim->coders[i].last_compile_mutex);
            long long last = sim->coders[i].last_compile_start;
            pthread_mutex_unlock(&sim->coders[i].last_compile_mutex);

            if (get_time_ms() - last > sim->time_to_burnout) {
                print_status(&sim->coders[i], "burned out");
                set_stop(sim, 1);
                return NULL;
            }

            pthread_mutex_lock(&sim->stop_mutex);

            if (sim->required_compiles == -1 || sim->coders[i].compile_count < sim->required_compiles)
                all_done = 0;

            pthread_mutex_unlock(&sim->stop_mutex);
        }
        if (sim->required_compiles != -1 && all_done) {
            set_stop(sim, 1);
            return NULL;
        }
        usleep(1000);
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 7)
		return 1;

    t_sim sim;

    sim.num_coders = atoi(argv[1]);
    sim.time_to_burnout = atoi(argv[2]);
    sim.time_to_compile = atoi(argv[3]);
    sim.time_to_debug = atoi(argv[4]);
    sim.time_to_refactor = atoi(argv[5]);
    sim.required_compiles = atoi(argv[6]);
    sim.stop = 0;
    sim.start_time = get_time_ms();

    pthread_mutex_init(&sim.stop_mutex, NULL);
    pthread_mutex_init(&sim.print_mutex, NULL);

    sim.dongles = malloc(sizeof(t_dongle) * sim.num_coders);
    sim.coders = malloc(sizeof(t_coder) * sim.num_coders);

    for (int i = 0; i < sim.num_coders; i++)
		pthread_mutex_init(&sim.dongles[i].mutex, NULL);

    for (int i = 0; i < sim.num_coders; i++) {
        sim.coders[i].id = i + 1;
        sim.coders[i].compile_count = 0;
        sim.coders[i].sim = &sim;
        sim.coders[i].left = &sim.dongles[i];
        sim.coders[i].right = &sim.dongles[(i + 1) % sim.num_coders];
        sim.coders[i].last_compile_start = sim.start_time;
        pthread_mutex_init(&sim.coders[i].last_compile_mutex, NULL);
    }

	// we have only one monitor
    pthread_create(&sim.monitor, NULL, monitor_routine, &sim);

    for (int i = 0; i < sim.num_coders; i++)
		pthread_create(&sim.coders[i].thread, NULL, coder_routine, &sim.coders[i]);

    pthread_join(sim.monitor, NULL);

    for (int i = 0; i < sim.num_coders; i++)
		pthread_join(sim.coders[i].thread, NULL);

    free(sim.dongles);
	free(sim.coders);

    return 0;
}
