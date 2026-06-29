/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:33:37 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 09:34:44 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static int	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->stop_mutex, NULL)
		|| pthread_mutex_init(&sim->print_mutex, NULL)
		|| pthread_mutex_init(&sim->seq_mutex, NULL))
		return (1);
	return (0);
}

static int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].available_at = 0;
		if (init_heap(&sim->dongles[i].queue,
				sim->number_of_coders))
			return (1);
		i++;
	}
	return (0);
}

static int	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->number_of_coders];
		sim->coders[i].last_compile_start = sim->start_time;
		pthread_mutex_init(&sim->coders[i].last_compile_mutex, NULL);
		i++;
	}
	return (0);
}

int	init_simulation(t_sim *sim)
{
	sim->stop = 0;
	sim->global_sequence = 0;
	if (init_mutexes(sim))
		return (1);
	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->dongles || !sim->coders)
		return (1);
	if (init_dongles(sim))
		return (1);
	sim->start_time = get_time_ms();
	return (init_coders(sim));
}

int	parse_args(int argc, char **argv, t_sim *sim)
{
	if (argc != 9)
		return (1);
	sim->number_of_coders = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->number_of_compiles_required = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	if (sim->number_of_coders <= 0
		|| sim->time_to_burnout <= 0
		|| sim->time_to_compile < 0
		|| sim->time_to_debug < 0
		|| sim->time_to_refactor < 0
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
