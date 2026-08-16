/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_release.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:40:44 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:40:48 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongle.h"
#include "logger.h"
#include <pthread.h>

#define DONGLE_WAIT_CAP_MS 1000

static void	dongle_release_one(t_dongle *dongle, long now, long cooldown)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->owner = -1;
	dongle->available_at = now + cooldown;
	pthread_mutex_unlock(&dongle->mutex);
}

void	dongles_release_both(t_dongle *a, t_dongle *b, long cooldown)
{
	long	now;

	now = get_current_time_ms();
	if (a->id < b->id || a == b)
	{
		dongle_release_one(a, now, cooldown);
		if (a != b)
			dongle_release_one(b, now, cooldown);
	}
	else
	{
		dongle_release_one(b, now, cooldown);
		dongle_release_one(a, now, cooldown);
	}
}

long	dongle_wake_time(t_dongle *dongle, long now)
{
	long	wake;

	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner == -1 && dongle->available_at > now)
		wake = dongle->available_at;
	else
		wake = now + DONGLE_WAIT_CAP_MS;
	pthread_mutex_unlock(&dongle->mutex);
	return (wake);
}
