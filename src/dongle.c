#include "dongle.h"
#include "logger.h"
#include <pthread.h>

int	dongle_init(t_dongle *dongle, int id)
{
	dongle->id = id;
	dongle->available_at = 0;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (0);
	return (1);
}

void	dongle_destroy(t_dongle *dongle)
{
	pthread_mutex_destroy(&dongle->mutex);
}

int	dongle_can_be_acquired(t_dongle *dongle)
{
	int	can_acquire;

	pthread_mutex_lock(&dongle->mutex);
	if (get_current_time_ms() >= dongle->available_at)
		can_acquire = 1;
	else
		can_acquire = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (can_acquire);
}

int	dongle_release(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available_at = get_current_time_ms();
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}
