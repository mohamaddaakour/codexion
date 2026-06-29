#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct s_sim        t_sim;
typedef struct s_coder      t_coder;
typedef struct s_dongle     t_dongle;

struct s_dongle
{
    pthread_mutex_t mutex;
};

struct s_coder
{
    int             id;
    long long       last_compile_start;
    int             compile_count;
    pthread_t       thread;
    t_dongle        *left;
    t_dongle        *right;
    pthread_mutex_t last_compile_mutex;
    t_sim           *sim;
};

struct s_sim
{
    int             num_coders;
    long long       time_to_burnout;
    long long       time_to_compile;
    long long       time_to_debug;
    long long       time_to_refactor;
    int             required_compiles;
    long long       start_time;
    int             stop;
    pthread_mutex_t stop_mutex;
    pthread_mutex_t print_mutex;
    t_coder         *coders;
    t_dongle        *dongles;
    pthread_t       monitor;
};

long long get_time_ms(void) { struct timeval tv; gettimeofday(&tv, NULL); return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000)); }
void precise_sleep(long long ms) { long long s = get_time_ms(); while ((get_time_ms() - s) < ms) usleep(200); }
int get_stop(t_sim *sim) { int s; pthread_mutex_lock(&sim->stop_mutex); s = sim->stop; pthread_mutex_unlock(&sim->stop_mutex); return s; }
void set_stop(t_sim *sim, int val) { pthread_mutex_lock(&sim->stop_mutex); sim->stop = val; pthread_mutex_unlock(&sim->stop_mutex); }

void print_status(t_coder *coder, char *msg)
{
    pthread_mutex_lock(&coder->sim->print_mutex);
    if (!get_stop(coder->sim) || strcmp(msg, "burned out") == 0)
        printf("%lld %d %s\n", get_time_ms() - coder->sim->start_time, coder->id, msg);
    pthread_mutex_unlock(&coder->sim->print_mutex);
}

void take_dongles(t_coder *c)
{
    if (c->sim->num_coders == 1)
    {
        pthread_mutex_lock(&c->left->mutex);
        print_status(c, "has taken a dongle");
        precise_sleep(c->sim->time_to_burnout + 10);
        return;
    }
    // Prevent deadlocks by sorting resource memory addresses
    if (c->left < c->right) {
        pthread_mutex_lock(&c->left->mutex);
        print_status(c, "has taken a dongle");
        pthread_mutex_lock(&c->right->mutex);
        print_status(c, "has taken a dongle");
    } else {
        pthread_mutex_lock(&c->right->mutex);
        print_status(c, "has taken a dongle");
        pthread_mutex_lock(&c->left->mutex);
        print_status(c, "has taken a dongle");
    }
}

void release_dongles(t_coder *c)
{
    if (c->sim->num_coders == 1) {
        pthread_mutex_unlock(&c->left->mutex);
        return;
    }
    pthread_mutex_unlock(&c->left->mutex);
    pthread_mutex_unlock(&c->right->mutex);
}

void *coder_routine(void *arg)
{
    t_coder *c = (t_coder *)arg;
    if (c->id % 2 == 0) usleep(1000);
    while (!get_stop(c->sim))
    {
        take_dongles(c);
        if (get_stop(c->sim)) break;
        pthread_mutex_lock(&c->last_compile_mutex);
        c->last_compile_start = get_time_ms();
        pthread_mutex_unlock(&c->last_compile_mutex);
        print_status(c, "is compiling");
        precise_sleep(c->sim->time_to_compile);
        pthread_mutex_lock(&c->sim->stop_mutex);
        c->compile_count++;
        pthread_mutex_unlock(&c->sim->stop_mutex);
        release_dongles(c);
        if (get_stop(c->sim)) break;
        print_status(c, "is debugging");
        precise_sleep(c->sim->time_to_debug);
        if (get_stop(c->sim)) break;
        print_status(c, "is refactoring");
        precise_sleep(c->sim->time_to_refactor);
    }
    return (NULL);
}

void *monitor_routine(void *arg)
{
    t_sim *sim = (t_sim *)arg;
    while (!get_stop(sim))
    {
        int all_done = 1;
        for (int i = 0; i < sim->num_coders; i++)
        {
            pthread_mutex_lock(&sim->coders[i].last_compile_mutex);
            long long last = sim->coders[i].last_compile_start;
            pthread_mutex_unlock(&sim->coders[i].last_compile_mutex);
            if (get_time_ms() - last > sim->time_to_burnout)
            {
                print_status(&sim->coders[i], "burned out");
                set_stop(sim, 1);
                return (NULL);
            }
            pthread_mutex_lock(&sim->stop_mutex);
            if (sim->required_compiles == -1 || sim->coders[i].compile_count < sim->required_compiles)
                all_done = 0;
            pthread_mutex_unlock(&sim->stop_mutex);
        }
        if (sim->required_compiles != -1 && all_done)
        {
            set_stop(sim, 1);
            return (NULL);
        }
        usleep(1000);
    }
    return (NULL);
}

int main(int argc, char **argv)
{
    t_sim sim;
    if (argc < 7) return 1;
    sim.num_coders = atoi(argv[1]);
    sim.time_to_burnout = atoi(argv[2]);
    sim.time_to_compile = atoi(argv[3]);
    sim.time_to_debug = atoi(argv[4]);
    sim.time_to_refactor = atoi(argv[5]);
    sim.required_compiles = atoi(argv[6]);
    sim.stop = 0;

    pthread_mutex_init(&sim.stop_mutex, NULL);
    pthread_mutex_init(&sim.print_mutex, NULL);
    sim.dongles = malloc(sizeof(t_dongle) * sim.num_coders);
    sim.coders = malloc(sizeof(t_coder) * sim.num_coders);
    sim.start_time = get_time_ms();

    for (int i = 0; i < sim.num_coders; i++)
        pthread_mutex_init(&sim.dongles[i].mutex, NULL);

    for (int i = 0; i < sim.num_coders; i++)
    {
        sim.coders[i].id = i + 1;
        sim.coders[i].compile_count = 0;
        sim.coders[i].sim = &sim;
        sim.coders[i].left = &sim.dongles[i];
        sim.coders[i].right = &sim.dongles[(i + 1) % sim.num_coders];
        sim.coders[i].last_compile_start = sim.start_time;
        pthread_mutex_init(&sim.coders[i].last_compile_mutex, NULL);
    }

    for (int i = 0; i < sim.num_coders; i++)
        pthread_create(&sim.coders[i].thread, NULL, coder_routine, &sim.coders[i]);
    pthread_create(&sim.monitor, NULL, monitor_routine, &sim);

    pthread_join(sim.monitor, NULL);
    for (int i = 0; i < sim.num_coders; i++)
        pthread_join(sim.coders[i].thread, NULL);

    for (int i = 0; i < sim.num_coders; i++)
    {
        pthread_mutex_destroy(&sim.dongles[i].mutex);
        pthread_mutex_destroy(&sim.coders[i].last_compile_mutex);
    }
    free(sim.dongles); free(sim.coders);
    pthread_mutex_destroy(&sim.stop_mutex); pthread_mutex_destroy(&sim.print_mutex);
    return (0);
}
