/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:44:34 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:44:47 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

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

void	sleep_ms(long ms)
{
	usleep(ms * 1000);
}

void	ms_to_timespec(long ms, struct timespec *ts)
{
	ts->tv_sec = ms / 1000;
	ts->tv_nsec = (ms % 1000) * 1000000L;
}
