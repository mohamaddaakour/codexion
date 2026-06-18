/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 13:45:24 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/16 11:37:23 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/codexion.h"

// we create for each coder a thread to let them works
// at the same time
// coder_routine is a function that every coder should execute
int	start_simulation(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
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
