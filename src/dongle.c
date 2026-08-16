/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
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

int	dongle_init(t_dongle *dongle, int id)
{
	dongle->id = id;
	dongle->available_at = 0;
	dongle->owner = -1;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (0);
	return (1);
}

void	dongle_destroy(t_dongle *dongle)
{
	pthread_mutex_destroy(&dongle->mutex);
}

int	dongle_is_free(t_dongle *dongle)
{
	int		available;
	long	now;

	pthread_mutex_lock(&dongle->mutex);
	now = get_current_time_ms();
	available = (dongle->owner == -1 && now >= dongle->available_at);
	pthread_mutex_unlock(&dongle->mutex);
	return (available);
}

void	dongle_claim(t_dongle *dongle, int coder_id)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->owner = coder_id;
	pthread_mutex_unlock(&dongle->mutex);
}

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
