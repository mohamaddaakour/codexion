/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:33:48 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 09:33:49 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

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
