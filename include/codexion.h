/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 10:53:57 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/16 11:42:12 by mdaakour         ###   ########.fr       */
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

// here we said trust me I will implement these structs
// later, because in s_coder we used s_sim but we didn't implement
// s_sim yet
typedef struct s_sim		t_sim;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;

// dongle structure
typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	long			available_at;
}	t_dongle;

// coder structure
typedef struct s_coder
{
	int				id;
	long			last_compile_start;
	int				compile_count;

	pthread_t		thread;

	t_dongle		*left;
	t_dongle		*right;

	t_sim			*sim;
}	t_coder;

// simulation structure
typedef struct s_sim
{
	int				num_coders;

	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;

	int				required_compiles;
	long			dongle_cooldown;

	int				scheduler;

	long			start_time;

	int				stop;

	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;

	t_coder			*coders;
	t_dongle		*dongles;

	pthread_t		monitor;

}	t_sim;

// helpers functions
int		ft_atoi(const char *str);
int		ft_isdigit(int c);

// time
long	get_time_ms(void);
void	precise_sleep(long ms);

// parsing
int		ft_isdigit(int c);
int		ft_atoi(const char *str);
int		is_valid_number(char *s);
long	parse_positive_long(char *s);
int		parse_args(int argc, char **argv, t_sim *sim);

int		init_simulation(t_sim *sim);
int		init_dongles(t_sim *sim);
int		init_coders(t_sim *sim);
void	set_start_time(t_sim *sim);
// void	destroy_sim(t_sim *sim);

void	*coder_routine(void *arg);
int		start_simulation(t_sim *sim);
void	join_coders(t_sim *sim);

int		get_stop(t_sim *sim);
void	set_stop(t_sim *sim, int value);

void	print_status(t_coder *coder, char *msg);

void	*monitor_routine(void *arg);

void	take_dongles(t_coder *coder);
void	release_dongles(t_coder *coder);
void	destroy_sim(t_sim *sim);

#endif
