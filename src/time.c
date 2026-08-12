/*
** gettimeofday-based timing helpers for serialized logging,
** milestone M3.
*/
#include "logger.h"
#include <sys/time.h>

long	get_current_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, 0) == 0)
		return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000L));
	return (0);
}

long	get_elapsed_time_ms(t_simulation *simulation)
{
	return (get_current_time_ms() - simulation->start_time);
}
