/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 13:45:46 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/16 11:26:49 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/codexion.h"

// this is the function that every coder should execute
void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;

	while (!get_stop(coder->sim))
	{
		take_dongles(coder);

		coder->last_compile_start = get_time_ms();

		print_status(coder, "is compiling");
		precise_sleep(coder->sim->time_to_compile);

		coder->compile_count++;

		release_dongles(coder);

		print_status(coder, "is debugging");
		precise_sleep(coder->sim->time_to_debug);

		print_status(coder, "is refactoring");
		precise_sleep(coder->sim->time_to_refactor);
	}
	return (NULL);
}
