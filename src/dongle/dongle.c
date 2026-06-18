#include "../include/codexion.h"

void	take_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->mutex);
	print_status(coder, "has taken left dongle");

	pthread_mutex_lock(&coder->right->mutex);
	print_status(coder, "has taken right dongle");
}

void	release_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->left->mutex);
	pthread_mutex_unlock(&coder->right->mutex);
}
