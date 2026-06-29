/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:52:38 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 15:03:46 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	acquire_dongle(t_dongle *d, t_coder *c)
{
	t_request	req;
	t_request	top;
	long long	now;

	req = create_request(c);
	pthread_mutex_lock(&d->mutex);
	heap_push(&d->queue, req, c->sim->scheduler);
	while (!get_stop(c->sim))
	{
		now = get_time_ms();
		top = heap_peek(&d->queue);
		if (top.coder_id == c->id && now >= d->available_at)
			break ;
		wait_dongle(d, now);
	}
	remove_request(d, c);
	pthread_mutex_unlock(&d->mutex);
}

void	release_dongle(t_dongle *d, t_sim *sim)
{
	pthread_mutex_lock(&d->mutex);
	d->available_at = get_time_ms() + sim->dongle_cooldown;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}

void	take_dongles(t_coder *c)
{
	if (c->sim->number_of_coders == 1)
	{
		take_one(c, c->left);
		precise_sleep(c->sim->time_to_burnout + 10);
		return ;
	}
	if (c->left < c->right)
	{
		take_one(c, c->left);
		take_one(c, c->right);
	}
	else
	{
		take_one(c, c->right);
		take_one(c, c->left);
	}
}

void	release_dongles(t_coder *c)
{
	release_dongle(c->left, c->sim);
	if (c->sim->number_of_coders > 1)
		release_dongle(c->right, c->sim);
}
