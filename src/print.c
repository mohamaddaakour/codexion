#include "../include/codexion.h"

void	print_status(t_coder *coder, char *msg)
{
	long	time;

	pthread_mutex_lock(&coder->sim->print_mutex);

	if (!get_stop(coder->sim))
	{
		time = get_time_ms() - coder->sim->start_time;
		printf("%ld %d %s\n", time, coder->id, msg);
	}

	pthread_mutex_unlock(&coder->sim->print_mutex);
}
