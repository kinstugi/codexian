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

static int	dongle_ready(t_dongle *dongle, long now)
{
	return (dongle->owner == -1 && now >= dongle->available_at);
}

static int	try_acquire_pair(t_dongle *first, t_dongle *second,
		int coder_id, long now)
{
	int	acquired;

	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	acquired = dongle_ready(first, now) && dongle_ready(second, now);
	if (acquired)
	{
		first->owner = coder_id;
		second->owner = coder_id;
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (acquired);
}

int	dongles_try_acquire_both(t_dongle *a, t_dongle *b, int coder_id)
{
	long	now;

	now = get_current_time_ms();
	if (a == b)
	{
		pthread_mutex_lock(&a->mutex);
		if (dongle_ready(a, now))
		{
			a->owner = coder_id;
			pthread_mutex_unlock(&a->mutex);
			return (1);
		}
		pthread_mutex_unlock(&a->mutex);
		return (0);
	}
	if (a->id < b->id)
		return (try_acquire_pair(a, b, coder_id, now));
	return (try_acquire_pair(b, a, coder_id, now));
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
