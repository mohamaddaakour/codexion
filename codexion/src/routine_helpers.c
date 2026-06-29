/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:33:43 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 15:07:15 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	update_compile_time(t_coder *c)
{
	pthread_mutex_lock(&c->last_compile_mutex);
	c->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&c->last_compile_mutex);
}

void	wake_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

void	compile_action(t_coder *c)
{
	update_compile_time(c);
	print_status(c, "is compiling");
	precise_sleep(c->sim->time_to_compile);
	pthread_mutex_lock(&c->sim->stop_mutex);
	c->compile_count++;
	pthread_mutex_unlock(&c->sim->stop_mutex);
}

int	check_burnout(t_sim *sim, int i)
{
	long long	last;

	pthread_mutex_lock(&sim->coders[i].last_compile_mutex);
	last = sim->coders[i].last_compile_start;
	pthread_mutex_unlock(&sim->coders[i].last_compile_mutex);
	if (get_time_ms() - last > sim->time_to_burnout)
	{
		print_status(&sim->coders[i], "burned out");
		set_stop(sim, 1);
		wake_dongles(sim);
		return (1);
	}
	return (0);
}

int	check_finished(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		if (sim->number_of_compiles_required != -1
			&& sim->coders[i].compile_count
			< sim->number_of_compiles_required)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			return (0);
		}
		pthread_mutex_unlock(&sim->stop_mutex);
		i++;
	}
	return (1);
}
