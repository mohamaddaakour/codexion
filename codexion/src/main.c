/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:33:40 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 09:33:40 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static int	create_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]))
			return (1);
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim))
		return (1);
	return (0);
}

static void	join_threads(t_sim *sim)
{
	int	i;

	pthread_join(sim->monitor, NULL);
	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	memset(&sim, 0, sizeof(t_sim));
	if (parse_args(argc, argv, &sim))
	{
		printf("Invalid args\n");
		return (1);
	}
	if (init_simulation(&sim))
	{
		printf("Init failed\n");
		destroy_sim(&sim);
		return (1);
	}
	if (create_threads(&sim))
	{
		destroy_sim(&sim);
		return (1);
	}
	join_threads(&sim);
	destroy_sim(&sim);
	return (0);
}
