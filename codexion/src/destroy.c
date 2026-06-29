/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:30:14 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 09:30:49 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

#include "../include/codexion.h"

static void	destroy_dongles(t_sim *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		free(sim->dongles[i].queue.nodes);
		i++;
	}
	free(sim->dongles);
}

static void	destroy_coders(t_sim *sim)
{
	int	i;

	if (!sim->coders)
		return ;
	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].last_compile_mutex);
		i++;
	}
	free(sim->coders);
}

static void	destroy_mutexes(t_sim *sim)
{
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->seq_mutex);
}

void	destroy_sim(t_sim *sim)
{
	if (!sim)
		return ;
	destroy_dongles(sim);
	destroy_coders(sim);
	destroy_mutexes(sim);
}
