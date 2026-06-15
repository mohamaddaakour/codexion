/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 11:13:23 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/15 13:45:37 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (parse_args(argc, argv, &sim))
		return (printf("Invalid args\n"), 1);
	if (init_simulation(&sim))
		return (printf("Init failed\n"), 1);
	if (start_simulation(&sim))
		return (printf("Thread start failed\n"), 1);
	join_coders(&sim);
	return (0);
}
