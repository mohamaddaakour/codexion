/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:33:45 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 09:33:46 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	precise_sleep(long long ms)
{
	long long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < ms)
		usleep(200);
}
