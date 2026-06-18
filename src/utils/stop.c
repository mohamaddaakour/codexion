#include "../../include/codexion.h"

// stop is used to stop the simulation
// pthread_mutex_lock we use it to lock the mutex
// this means no other thread can access this sim->stop
// pthread_mutex_unlock to unlock the mutex
int	get_stop(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->stop_mutex);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stop);
}

// to set stop
void	set_stop(t_sim *sim, int value)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = value;
	pthread_mutex_unlock(&sim->stop_mutex);
}
