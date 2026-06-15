/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 11:47:13 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/15 12:27:57 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

// to check if a number is a valid number totally
int	is_valid_number(char *s)
{
	int	i;

	i = 0;
	if (!s || !s[0])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

// parse long positive numbers
long	parse_positive_long(char *s)
{
	long	val;

	if (!is_valid_number(s))
		return (-1);
	val = ft_atoi(s);
	if (val <= 0)
		return (-1);
	return (val);
}

// parsing arguments function
// we used *sim not sim because we want to modify the
// sim in the main not just a copy from it
int	parse_args(int argc, char **argv, t_sim *sim)
{
	if (argc != 9)
		return (1);
	sim->num_coders = parse_positive_long(argv[1]);
	sim->time_to_burnout = parse_positive_long(argv[2]);
	sim->time_to_compile = parse_positive_long(argv[3]);
	sim->time_to_debug = parse_positive_long(argv[4]);
	sim->time_to_refactor = parse_positive_long(argv[5]);
	sim->required_compiles = parse_positive_long(argv[6]);
	sim->dongle_cooldown = parse_positive_long(argv[7]);
	if (sim->num_coders <= 0
		|| sim->time_to_burnout < 0
		|| sim->time_to_compile < 0
		|| sim->time_to_debug < 0
		|| sim->time_to_refactor < 0
		|| sim->required_compiles < 0
		|| sim->dongle_cooldown < 0)
		return (1);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = EDF;
	else
		return (1);
	return (0);
}
