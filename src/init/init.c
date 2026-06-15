/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 13:46:19 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/15 13:48:21 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/codexion.h"

// we set the start time
void	set_start_time(t_sim *sim)
{
	sim->start_time = get_time_ms();
}

// initialize the dongles
// number of dongles equals number of coders
// we create an array of dongles
// with pthread_mutex_init each dongle is a locked shared resource
// (no 2 coders with same dongle)
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
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].sim = sim;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->num_coders];
		i++;
	}
	return (0);
}

// initialize simulator
int	init_simulation(t_sim *sim)
{
	if (init_dongles(sim))
		return (1);
	set_start_time(sim);
	if (init_coders(sim))
		return (1);
	return (0);
}
