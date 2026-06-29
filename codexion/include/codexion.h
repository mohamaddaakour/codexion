/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:29:35 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 15:09:59 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

# define FIFO 0
# define EDF 1

typedef struct s_sim		t_sim;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;

typedef struct s_request
{
	int			coder_id;
	long long	sequence;
	long long	deadline;
}	t_request;

typedef struct s_heap
{
	t_request	*nodes;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long long		available_at;
	t_heap			queue;
}	t_dongle;

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

typedef struct s_sim
{
	int				number_of_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				number_of_compiles_required;
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

long long	get_time_ms(void);
void		precise_sleep(long long ms);

int			get_stop(t_sim *sim);
void		set_stop(t_sim *sim, int stop_value);
void		print_status(t_coder *coder, char *message);

void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);

void		take_dongles(t_coder *coder);
void		release_dongles(t_coder *coder);
void		wait_dongle(t_dongle *d, long long now);
void		acquire_dongle(t_dongle *d, t_coder *c);
void		take_one(t_coder *c, t_dongle *d);
void		remove_request(t_dongle *d, t_coder *c);
void		wake_dongles(t_sim *sim);

int			init_heap(t_heap *heap, int capacity);
int			compare_req(t_request a, t_request b, int scheduler);
void		heap_push(t_heap *heap, t_request req, int scheduler);
void		heap_pop(t_heap *heap, int scheduler);
t_request	heap_peek(t_heap *heap);
void		heap_remove(t_heap *heap, int coder_id, int scheduler);
void		swap_req(t_request *a, t_request *b);
void		heapify_down(t_heap *heap, int scheduler);
void		rebuild_heap(t_heap *heap, int scheduler);

int			init_simulation(t_sim *sim);
int			parse_args(int argc, char **argv, t_sim *sim);
void		destroy_sim(t_sim *sim);
long long	get_next_sequence(t_sim *sim);

t_request	create_request(t_coder *c);

void		update_compile_time(t_coder *c);
void		compile_action(t_coder *c);
int			check_burnout(t_sim *sim, int i);
int			check_finished(t_sim *sim);

#endif
