#include "../include/codexion.h"

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;

	sim = (t_sim *)arg;

	while (!get_stop(sim))
	{
		i = 0;
		while (i < sim->num_coders)
		{
			if ((get_time_ms()
				- sim->coders[i].last_compile_start)
				> sim->time_to_burnout)
			{
				print_status(&sim->coders[i], "burned out");
				set_stop(sim, 1);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
