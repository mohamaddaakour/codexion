#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FIFO 0
#define EDF 1

typedef struct s_sim        t_sim;
typedef struct s_coder      t_coder;
typedef struct s_dongle     t_dongle;

/* Heap Node for Scheduling Requests */
typedef struct s_request
{
	int			coder_id;
	long long	sequence;
	long long	deadline;
}	t_request;

/* Custom Heap Priority Queue Structure */
typedef struct s_heap
{
	t_request	*nodes;
	int			size;
	int			capacity;
}	t_heap;

/* Dongle Structure with Queue Orchestration */
typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long long		available_at;
	t_heap			queue;
}	t_dongle;

/* Coder Structure */
typedef struct s_coder
{
	int				id;
	long long		last_compile_start;
	int				compile_count;
	pthread_t		thread;
	t_dongle		*left;
	t_dongle		*right;
	pthread_mutex_t	last_compile_mutex;
	t_sim			*sim;
}	t_coder;

/* Main Simulation State */
typedef struct s_sim
{
	int				num_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				required_compiles;
	long long		dongle_cooldown;
	int				scheduler;
	long long		start_time;
	int				stop;
	long long		global_sequence;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	seq_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor;
}	t_sim;

/* Prototypes */
long long	get_time_ms(void);
void		precise_sleep(long long ms);
int			get_stop(t_sim *sim);
void		set_stop(t_sim *sim, int value);
void		print_status(t_coder *coder, char *msg);
long long	get_next_sequence(t_sim *sim);

/* --- Custom Min-Heap Implementations --- */
int	init_heap(t_heap *heap, int capacity)
{
	heap->nodes = malloc(sizeof(t_request) * capacity);
	if (!heap->nodes)
		return (1);
	heap->size = 0;
	heap->capacity = capacity;
	return (0);
}

int	compare_req(t_request a, t_request b, int scheduler)
{
	if (scheduler == EDF)
	{
		if (a.deadline < b.deadline)
			return (-1);
		if (a.deadline > b.deadline)
			return (1);
	}
	if (a.sequence < b.sequence)
		return (-1);
	if (a.sequence > b.sequence)
		return (1);
	return (0);
}

void	heap_push(t_heap *heap, t_request req, int scheduler)
{
	int			i;
	t_request	tmp;

	if (heap->size >= heap->capacity)
		return ;
	i = heap->size;
	heap->nodes[i] = req;
	heap->size++;
	while (i > 0)
	{
		int parent = (i - 1) / 2;
		if (compare_req(heap->nodes[i], heap->nodes[parent], scheduler) < 0)
		{
			tmp = heap->nodes[i];
			heap->nodes[i] = heap->nodes[parent];
			heap->nodes[parent] = tmp;
			i = parent;
		}
		else
			break;
	}
}

void	heap_pop(t_heap *heap, int scheduler)
{
	int			i;
	int			smallest;
	t_request	tmp;

	if (heap->size <= 0)
		return ;
	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];
	i = 0;
	while (2 * i + 1 < heap->size)
	{
		int left = 2 * i + 1;
		int right = 2 * i + 2;
		smallest = left;
		if (right < heap->size && compare_req(heap->nodes[right], heap->nodes[left], scheduler) < 0)
			smallest = right;
		if (compare_req(heap->nodes[smallest], heap->nodes[i], scheduler) < 0)
		{
			tmp = heap->nodes[i];
			heap->nodes[i] = heap->nodes[smallest];
			heap->nodes[smallest] = tmp;
			i = smallest;
		}
		else
			break;
	}
}

t_request	heap_peek(t_heap *heap)
{
	if (heap->size > 0)
		return (heap->nodes[0]);
	t_request null_req = {0, 0, 0};
	return (null_req);
}

void	heap_remove(t_heap *heap, int coder_id, int scheduler)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->nodes[i].coder_id == coder_id)
			break ;
		i++;
	}
	if (i == heap->size)
		return ;
	heap->nodes[i] = heap->nodes[heap->size - 1];
	heap->size--;
	// Re-heapify simple logic for safety
	t_heap tmp_heap;
	init_heap(&tmp_heap, heap->capacity);
	int j = 0;
	while (j < heap->size)
	{
		heap_push(&tmp_heap, heap->nodes[j], scheduler);
		j++;
	}
	free(heap->nodes);
	heap->nodes = tmp_heap.nodes;
	heap->size = tmp_heap.size;
}

/* --- Dongle Request Orchestrations --- */
void	acquire_dongle(t_dongle *d, t_coder *c)
{
	t_request	req;
	long long	now;

	pthread_mutex_lock(&d->mutex);
	req.coder_id = c->id;
	req.sequence = get_next_sequence(c->sim);
	pthread_mutex_lock(&c->last_compile_mutex);
	req.deadline = c->last_compile_start + c->sim->time_to_burnout;
	pthread_mutex_unlock(&c->last_compile_mutex);

	heap_push(&d->queue, req, c->sim->scheduler);
	while (!get_stop(c->sim))
	{
		now = get_time_ms();
		t_request top = heap_peek(&d->queue);
		if (top.coder_id == c->id && now >= d->available_at)
			break ;
		if (top.coder_id == c->id && d->available_at > now)
		{
			struct timespec ts;
			ts.tv_sec = d->available_at / 1000;
			ts.tv_nsec = (d->available_at % 1000) * 1000000;
			pthread_cond_timedwait(&d->cond, &d->mutex, &ts);
		}
		else
		{
			pthread_cond_wait(&d->cond, &d->mutex);
		}
	}
	if (!get_stop(c->sim))
		heap_pop(&d->queue, c->sim->scheduler);
	else
		heap_remove(&d->queue, c->id, c->sim->scheduler);
	pthread_mutex_unlock(&d->mutex);
}

void	release_dongle(t_dongle *d, t_sim *sim)
{
	pthread_mutex_lock(&d->mutex);
	d->available_at = get_time_ms() + sim->dongle_cooldown;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}

/* --- Core Logic Threads & Monitor --- */
void	take_dongles(t_coder *c)
{
	if (c->sim->num_coders == 1)
	{
		acquire_dongle(c->left, c);
		print_status(c, "has taken a dongle");
		precise_sleep(c->sim->time_to_burnout + 10);
		return ;
	}
	if (c->left < c->right)
	{
		acquire_dongle(c->left, c);
		print_status(c, "has taken a dongle");
		acquire_dongle(c->right, c);
		print_status(c, "has taken a dongle");
	}
	else
	{
		acquire_dongle(c->right, c);
		print_status(c, "has taken a dongle");
		acquire_dongle(c->left, c);
		print_status(c, "has taken a dongle");
	}
}

void	release_dongles(t_coder *c)
{
	if (c->sim->num_coders == 1)
	{
		release_dongle(c->left, c->sim);
		return ;
	}
	release_dongle(c->left, c->sim);
	release_dongle(c->right, c->sim);
}

void	*coder_routine(void *arg)
{
	t_coder	*c;

	c = (t_coder *)arg;
	if (c->id % 2 == 0)
		usleep(1000);
	while (!get_stop(c->sim))
	{
		take_dongles(c);
		if (get_stop(c->sim))
			break ;
		pthread_mutex_lock(&c->last_compile_mutex);
		c->last_compile_start = get_time_ms();
		pthread_mutex_unlock(&c->last_compile_mutex);
		print_status(c, "is compiling");
		precise_sleep(c->sim->time_to_compile);
		pthread_mutex_lock(&c->sim->stop_mutex);
		c->compile_count++;
		pthread_mutex_unlock(&c->sim->stop_mutex);
		release_dongles(c);
		if (get_stop(c->sim))
			break ;
		print_status(c, "is debugging");
		precise_sleep(c->sim->time_to_debug);
		if (get_stop(c->sim))
			break ;
		print_status(c, "is refactoring");
		precise_sleep(c->sim->time_to_refactor);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;
	int			i;
	long long	last;
	int			all_done;

	sim = (t_sim *)arg;
	while (!get_stop(sim))
	{
		all_done = 1;
		i = 0;
		while (i < sim->num_coders)
		{
			pthread_mutex_lock(&sim->coders[i].last_compile_mutex);
			last = sim->coders[i].last_compile_start;
			pthread_mutex_unlock(&sim->coders[i].last_compile_mutex);
			if (get_time_ms() - last > sim->time_to_burnout)
			{
				print_status(&sim->coders[i], "burned out");
				set_stop(sim, 1);
				// Wake up threads waiting on dongles to close cleanly
				for (int d = 0; d < sim->num_coders; d++) {
					pthread_mutex_lock(&sim->dongles[d].mutex);
					pthread_cond_broadcast(&sim->dongles[d].cond);
					pthread_mutex_unlock(&sim->dongles[d].mutex);
				}
				return (NULL);
			}
			pthread_mutex_lock(&sim->stop_mutex);
			if (sim->required_compiles == -1 || sim->coders[i].compile_count < sim->required_compiles)
				all_done = 0;
			pthread_mutex_unlock(&sim->stop_mutex);
			i++;
		}
		if (sim->required_compiles != -1 && all_done)
		{
			set_stop(sim, 1);
			for (int d = 0; d < sim->num_coders; d++) {
				pthread_mutex_lock(&sim->dongles[d].mutex);
				pthread_cond_broadcast(&sim->dongles[d].cond);
				pthread_mutex_unlock(&sim->dongles[d].mutex);
			}
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

/* --- System Helpers & Synchronizers --- */
int	get_stop(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->stop_mutex);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stop);
}

void	set_stop(t_sim *sim, int value)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = value;
	pthread_mutex_unlock(&sim->stop_mutex);
}

long long	get_next_sequence(t_sim *sim)
{
	long long	seq;

	pthread_mutex_lock(&sim->seq_mutex);
	seq = sim->global_sequence++;
	pthread_mutex_unlock(&sim->seq_mutex);
	return (seq);
}

void	print_status(t_coder *coder, char *msg)
{
	long long	time;

	pthread_mutex_lock(&coder->sim->print_mutex);
	if (!get_stop(coder->sim) || strcmp(msg, "burned out") == 0)
	{
		time = get_time_ms() - coder->sim->start_time;
		printf("%lld %d %s\n", time, coder->id, msg);
	}
	pthread_mutex_unlock(&coder->sim->print_mutex);
}

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	precise_sleep(long long ms)
{
	long long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < ms)
		usleep(200);
}

/* --- Intialization & Cleanup --- */
void	destroy_sim(t_sim *sim)
{
	int	i;

	if (!sim)
		return ;
	if (sim->dongles)
	{
		i = 0;
		while (i < sim->num_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			pthread_cond_destroy(&sim->dongles[i].cond);
			free(sim->dongles[i].queue.nodes);
			i++;
		}
		free(sim->dongles);
	}
	if (sim->coders)
	{
		i = 0;
		while (i < sim->num_coders)
		{
			pthread_mutex_destroy(&sim->coders[i].last_compile_mutex);
			i++;
		}
		free(sim->coders);
	}
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->seq_mutex);
}

int	init_simulation(t_sim *sim)
{
	int	i;

	if (pthread_mutex_init(&sim->stop_mutex, NULL) ||
		pthread_mutex_init(&sim->print_mutex, NULL) ||
		pthread_mutex_init(&sim->seq_mutex, NULL))
		return (1);
	sim->stop = 0;
	sim->global_sequence = 0;
	sim->dongles = malloc(sizeof(t_dongle) * sim->num_coders);
	sim->coders = malloc(sizeof(t_coder) * sim->num_coders);
	if (!sim->dongles || !sim->coders)
		return (1);
	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].available_at = 0;
		if (init_heap(&sim->dongles[i].queue, sim->num_coders))
			return (1);
		i++;
	}
	sim->start_time = get_time_ms();
	i = 0;
	while (i < sim->num_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->num_coders];
		sim->coders[i].last_compile_start = sim->start_time;
		pthread_mutex_init(&sim->coders[i].last_compile_mutex, NULL);
		i++;
	}
	return (0);
}

int	parse_args(int argc, char **argv, t_sim *sim)
{
	if (argc != 9)
		return (1);
	sim->num_coders = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->required_compiles = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	if (sim->num_coders <= 0 || sim->time_to_burnout <= 0 || sim->time_to_compile < 0
		|| sim->time_to_debug < 0 || sim->time_to_refactor < 0 || sim->dongle_cooldown < 0)
		return (1);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = EDF;
	else
		return (1);
	return (0);
}

int	main(int argc, char **argv)
{
	t_sim	sim;
	int		i;

	memset(&sim, 0, sizeof(t_sim));
	if (parse_args(argc, argv, &sim))
	{
		printf("Invalid args\n");
		return (1);
	}
	if (init_simulation(&sim))
	{
		printf("Init failed\n");
		destroy_sim(&sim);
		return (1);
	}
	i = 0;
	while (i < sim.num_coders)
	{
		pthread_create(&sim.coders[i].thread, NULL, coder_routine, &sim.coders[i]);
		i++;
	}
	pthread_create(&sim.monitor, NULL, monitor_routine, &sim);
	pthread_join(sim.monitor, NULL);
	i = 0;
	while (i < sim.num_coders)
	{
		pthread_join(sim.coders[i].thread, NULL);
		i++;
	}
	destroy_sim(&sim);
	return (0);
}
