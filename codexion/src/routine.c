/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_helper.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:52:44 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 09:52:45 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

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
		compile_action(c);
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

	sim = (t_sim *)arg;
	while (!get_stop(sim))
	{
		i = 0;
		while (i < sim->number_of_coders)
		{
			if (check_burnout(sim, i))
				return (NULL);
			i++;
		}
		if (check_finished(sim))
		{
			set_stop(sim, 1);
			wake_dongles(sim);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
