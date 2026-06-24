# **Definition**

- A project in 42 core curriculum done by mdaakour, the main idea of this project is to build an application using multithreading (pthread) to process multiple tasks in the same time.

- In this project we have many coders want to compile specific amount of compiles for each coder before burn out, and they can't compile until they have two dongles left and right.


# **How to run the code**

```shell
make

./codexion 3 200 200 200 200 3
```


# **Code Explanation**

- Thread: A thread is the smallest unit of execution within an operating system. It represents a single, independent sequence of instructions that a CPU can process. Multiple threads can exist within a single program (or process) and execute tasks simultaneously.

- Per example: here we have many coders that works in the same time, so each one of them is a thread.

- last_compile_start is to track the time when the coder last time compiled to check if he will burn out.

- If the required_compiles equals -1 this means the program will still working until all coders finish its compiles (we don't have burnout).

- If stop is 1 we have to stop the program, if 0 the program will continue.


```c
// we have to use this library to enable pthreads methods
#include <pthread.h>
```

```c
// to declare this as a thread
pthread_t thread;

// to declare this as a mutex (lock)
pthread_mutex_t last_compile_mutex;
```

```c
// like this we locked the stop and no one can access it until
// we unlock it
int get_stop(t_sim *sim) {
    int stop;
    pthread_mutex_lock(&sim->stop_mutex);
    stop = sim->stop;
    pthread_mutex_unlock(&sim->stop_mutex);
    return (stop);
}
```

```c
// to initialize the lock we put it in main
pthread_mutex_init(&sim.stop_mutex, NULL);
```

```c
// this is coder routine every coder should execut it
// the return type should be generic pointer and the parameter type
// should be a generic pointer
void *coder_routine(void *arg)
{
    // casting
    t_coder *c = (t_coder *)arg;

    // always when we have loop in multithreading means this loop will still
    // working for all threads until the program finish
    while (!get_stop(c->sim))
    {
        pthread_mutex_lock(&c->last_compile_mutex);
        c->last_compile_start = get_time_ms();
        pthread_mutex_unlock(&c->last_compile_mutex);

        print_status(c, "is compiling");
        precise_sleep(c->sim->time_to_compile);
    }

    // always we return NULL in the routine
    return (NULL);
}
```

```c
// to create a thread
pthread_create(&sim.coders[i].thread, NULL, coder_routine, &sim.coders[i]);
```

```c
// we have to join all the releated threads, to prevent the program from craching
// without finishing
pthread_join(sim.coders[i].thread, NULL);
```

```c
// to destroy the mutexes
pthread_mutex_destroy(&sim.coders[i].last_compile_mutex);
```

```c
#include <sys/time.h>

// this function to take the time in milleseconds
long long get_time_ms(void) {
    // we create a struct
    struct timeval tv;

    // now we can use the tv_sec and tv_usec
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}
```

```c
#include <unistd.h>

// to make the program sleep for 200 microseconds
usleep(200);
```

```c
// will fill the struct values with 0's
memset(&sim, 0, sizeof(t_sim));
```

```c
// we allocate memory for an array of coders
sim.coders = (t_coder *)malloc(sizeof(t_coder) * sim.num_coders);
```

```c
// to free the allocated memory
free(sim.coders);
```

```c
// always when we have something we want to edit or modify it in the main
// not just a copy from it we put it pointer
int get_stop(t_sim *sim)

// but in the main it will be like that
t_sim sim;
```

```c
// we have to declare the struct first
// because maybe we have 2 structures that take something from each other
// like that we will get an error, so if we declare it like that
// we say trust me this struct will be implemented later and we prevent errors
typedef struct s_coder t_coder;

// like this we create a structure
typedef struct s_coder {
    int coder_id;
    long long last_compile_start;
    int compil_count;
    pthread_t coder_thread;
    pthread_mutex_t last_compile_mutex;
} t_coder;
```

```c
// this will return 0 if the msg equals the string
// otherwise the 2 strings are not the same
strcmp(msg, "burned out")
```