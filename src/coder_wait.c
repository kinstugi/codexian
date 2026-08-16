/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_wait.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:39:22 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:39:28 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"
#include "dongle.h"
#include "logger.h"
#include "scheduler.h"
#include <pthread.h>

int	coder_wait_ms(t_simulation *simulation, long ms)
{
	struct timespec	deadline;
	long			end;
	int			stopped;

	end = get_current_time_ms() + ms;
	ms_to_timespec(end, &deadline);
	pthread_mutex_lock(&simulation->sync.mutex);
	while (!simulation->stop_flag && get_current_time_ms() < end)
		pthread_cond_timedwait(&simulation->sync.condition,
			&simulation->sync.mutex, &deadline);
	stopped = simulation->stop_flag;
	pthread_mutex_unlock(&simulation->sync.mutex);
	return (!stopped);
}

static long	wait_deadline_ms(t_coder *coder)
{
	long	now;
	long	deadline;
	long	right_wake;

	now = get_current_time_ms();
	deadline = dongle_wake_time(coder->left_dongle, now);
	if (coder->right_dongle != coder->left_dongle)
	{
		right_wake = dongle_wake_time(coder->right_dongle, now);
		if (right_wake < deadline)
			deadline = right_wake;
	}
	return (deadline);
}

static void	wait_for_grant(t_coder *coder)
{
	t_simulation	*simulation;
	struct timespec	deadline;

	simulation = coder->simulation;
	ms_to_timespec(wait_deadline_ms(coder), &deadline);
	if (!simulation->stop_flag)
		pthread_cond_timedwait(&simulation->sync.condition,
			&simulation->sync.mutex, &deadline);
}

int	wait_until_granted(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	while (!coder->granted)
	{
		if (simulation->stop_flag)
			return (0);
		wait_for_grant(coder);
		scheduler_grant(simulation);
	}
	return (1);
}
