/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:31:31 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 15:03:08 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	wait_dongle(t_dongle *d, long long now)
{
	struct timespec	ts;

	if (d->available_at > now)
	{
		ts.tv_sec = d->available_at / 1000;
		ts.tv_nsec = (d->available_at % 1000) * 1000000;
		pthread_cond_timedwait(&d->cond, &d->mutex, &ts);
	}
	else
		pthread_cond_wait(&d->cond, &d->mutex);
}

t_request	create_request(t_coder *c)
{
	t_request	req;

	req.coder_id = c->id;
	req.sequence = get_next_sequence(c->sim);
	pthread_mutex_lock(&c->last_compile_mutex);
	req.deadline = c->last_compile_start
		+ c->sim->time_to_burnout;
	pthread_mutex_unlock(&c->last_compile_mutex);
	return (req);
}

void	remove_request(t_dongle *d, t_coder *c)
{
	if (!get_stop(c->sim))
		heap_pop(&d->queue, c->sim->scheduler);
	else
		heap_remove(&d->queue, c->id, c->sim->scheduler);
}

void	take_one(t_coder *c, t_dongle *d)
{
	acquire_dongle(d, c);
	print_status(c, "has taken a dongle");
}
