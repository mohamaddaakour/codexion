/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 13:20:54 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/16 14:46:32 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
// pthread_mutex_t mutex this is an object used to lock and unlock
typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	long			available_at;
}	t_dongle;

// coder structure
// pthread_t thread this is a variable to store the thread id
typedef struct s_coder
{
	int				id;
	long			last_compile_start;
	int				compile_count;

	pthread_t		thread;

	t_dongle		*left;
	t_dongle		*right;

	pthread_mutex_t	last_compile_mutex;

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

	pthread_mutex_t	start_mutex;
	int				start;

}	t_sim;

// helpers functions
int		ft_atoi(const char *str);
int		ft_isdigit(int c);

// time
long	get_time_ms(void);
void	precise_sleep(long ms);

void	join_threads(t_sim *sim);

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


// this is the function that every coder should execute
void	*coder_routine(void *arg)
{
	t_coder	*c;

	c = (t_coder *)arg;
	while (!get_stop(c->sim))
	{
		pthread_mutex_lock(&c->sim->start_mutex);
		if (c->sim->start)
		{
			pthread_mutex_unlock(&c->sim->start_mutex);
			break;
		}
		pthread_mutex_unlock(&c->sim->start_mutex);
		usleep(100);
	}
	while (!get_stop(c->sim))
	{
		take_dongles(c);
		pthread_mutex_lock(&c->last_compile_mutex);
		c->last_compile_start = get_time_ms();
		pthread_mutex_unlock(&c->last_compile_mutex);
		print_status(c, "is compiling");
		precise_sleep(c->sim->time_to_compile);
		c->compile_count++;
		release_dongles(c);
		print_status(c, "is debugging");
		precise_sleep(c->sim->time_to_debug);
		print_status(c, "is refactoring");
		precise_sleep(c->sim->time_to_refactor);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;
	long	last;

	sim = (t_sim *)arg;

	while (!get_stop(sim))
	{
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
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}







// stop is used to stop the simulation
// pthread_mutex_lock we use it to lock the mutex
// this means no other thread can access this sim->stop
// pthread_mutex_unlock to unlock the mutex
int	get_stop(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->stop_mutex);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stop);
}

// to set stop
void	set_stop(t_sim *sim, int value)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = value;
	pthread_mutex_unlock(&sim->stop_mutex);
}

// to print the status
void	print_status(t_coder *coder, char *msg)
{
	long	time;

	pthread_mutex_lock(&coder->sim->print_mutex);

	if (!get_stop(coder->sim))
	{
		time = get_time_ms() - coder->sim->start_time;
		printf("%ld %d %s\n", time, coder->id, msg);
	}

	pthread_mutex_unlock(&coder->sim->print_mutex);
}

// to destroy the simulation
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
			i++;
		}
		free(sim->dongles);
		sim->dongles = NULL;
	}

	if (sim->coders)
	{
		i = 0;
		while (i < sim->num_coders)
		{
			pthread_mutex_destroy(
				&sim->coders[i].last_compile_mutex);
			i++;
		}
		free(sim->coders);
		sim->coders = NULL;
	}

	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->start_mutex);
}

// to let a coder takes 2 dongles to start compiling
void	take_dongles(t_coder *c)
{
	t_dongle	*first;
	t_dongle	*second;

	if (c->sim->num_coders == 1)
	{
		pthread_mutex_lock(&c->left->mutex);
		print_status(c, "has taken left dongle");
		precise_sleep(c->sim->time_to_burnout);
		pthread_mutex_unlock(&c->left->mutex);
		return ;
	}

	if (c->left < c->right)
	{
		first = c->left;
		second = c->right;
	}
	else
	{
		first = c->right;
		second = c->left;
	}

	pthread_mutex_lock(&first->mutex);
	print_status(c, "has taken first dongle");

	pthread_mutex_lock(&second->mutex);
	print_status(c, "has taken second dongle");
}

// to put the dongles on the table
void	release_dongles(t_coder *c)
{
	pthread_mutex_unlock(&c->left->mutex);
	pthread_mutex_unlock(&c->right->mutex);
}

// initialize simulator
int	init_simulation(t_sim *sim)
{
	// to create a lock
	if (pthread_mutex_init(&sim->stop_mutex, NULL))
		return (1);
	if (pthread_mutex_init(&sim->print_mutex, NULL))
		return (1);
	if (pthread_mutex_init(&sim->start_mutex, NULL))
		return (1);

	sim->stop = 0;
	sim->start = 0;

	if (init_dongles(sim))
		return (1);

	set_start_time(sim);

	if (init_coders(sim))
		return (1);

	return (0);
}

// pthread_join is used like a manager and will wait
// all the thread to finish, without it the program
// may finish immediately before finishing anything
void	join_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

void	join_threads(t_sim *sim)
{
	int	i;

	pthread_join(sim->monitor, NULL);

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

// we create for each coder a thread to let them works
// at the same time
// coder_routine is a function that every coder should execute
int	start_simulation(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		// here we create a thread
		if (pthread_create(&sim->coders[i].thread,
				NULL,
				coder_routine,
				&sim->coders[i]))
			return (1);
		i++;
	}

	if (pthread_create(&sim->monitor,
			NULL,
			monitor_routine,
			sim))
		return (1);

	return (0);
}

// we set the start time
void	set_start_time(t_sim *sim)
{
	sim->start_time = get_time_ms();
}

// initialize the dongles
// number of dongles equals number of coders
// we create an array of dongles
// with pthread_mutex_init each dongle is a locked shared resource
// this means (no 2 coders with same dongle)
int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->num_coders);
	if (!sim->dongles)
		return (1);
	i = 0;
	while (i < sim->num_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL))
			return (1);
		sim->dongles[i].available_at = 0;
		i++;
	}
	return (0);
}

// initialize coders
// we create an array of coders
// each coder will use 2 dongles at his processing time
int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->num_coders);
	if (!sim->coders)
		return (1);

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

// check if this char is a digit
int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

// convert a number from string into an integer
int	ft_atoi(const char *str)
{
	long	result;
	int		sign;

	result = 0;
	sign = 1;
	while ((*str >= 9 && *str <= 13) || *str == ' ')
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (ft_isdigit(*str))
	{
		result = (result * 10) + (*str - '0');
		str++;
	}
	return ((int)(result * sign));
}

// to check if a number is a valid number totally
int	is_valid_number(char *s)
{
	int	i;

	i = 0;
	if (!s || !s[0])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

// parse long positive numbers
long	parse_positive_long(char *s)
{
	long	val;

	if (!is_valid_number(s))
		return (-1);
	val = ft_atoi(s);
	if (val <= 0)
		return (-1);
	return (val);
}

// parsing arguments function
// we used *sim not sim because we want to modify the
// sim in the main not just a copy from it
int	parse_args(int argc, char **argv, t_sim *sim)
{
	if (argc != 9)
		return (1);
	sim->num_coders = parse_positive_long(argv[1]);
	sim->time_to_burnout = parse_positive_long(argv[2]);
	sim->time_to_compile = parse_positive_long(argv[3]);
	sim->time_to_debug = parse_positive_long(argv[4]);
	sim->time_to_refactor = parse_positive_long(argv[5]);
	sim->required_compiles = parse_positive_long(argv[6]);
	sim->dongle_cooldown = parse_positive_long(argv[7]);
	if (sim->num_coders <= 0
		|| sim->time_to_burnout < 0
		|| sim->time_to_compile < 0
		|| sim->time_to_debug < 0
		|| sim->time_to_refactor < 0
		|| sim->required_compiles < 0
		|| sim->dongle_cooldown < 0)
		return (1);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = EDF;
	else
		return (1);
	return (0);
}

// we convert the time into milleseconds
// tv_sec is the time in seconds
// tv_usec is the time in microseconds
long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000));
}

// this function to make the program sleep
// this is more precise than using the usleep alone
// usleep(100) means sleep for 100 microseconds
void	precise_sleep(long ms)
{
	long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < ms)
		usleep(100);
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	memset(&sim, 0, sizeof(t_sim));

	if (parse_args(argc, argv, &sim))
		return (printf("Invalid args\n"), 1);

	if (init_simulation(&sim))
		return (printf("Init failed\n"), 1);

	if (start_simulation(&sim))
		return (printf("Thread start failed\n"), 1);

	pthread_mutex_lock(&sim.start_mutex);
	sim.start = 1;
	pthread_mutex_unlock(&sim.start_mutex);

	join_threads(&sim);
	destroy_sim(&sim);

	return (0);
}
