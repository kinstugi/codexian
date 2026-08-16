/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_dongles.c                                    :+:      :+:    :+:   */
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

static void	log_dongles_taken(t_coder *coder)
{
	logger(coder->simulation, coder->id, "has taken a dongle");
	if (coder->left_dongle != coder->right_dongle)
		logger(coder->simulation, coder->id, "has taken a dongle");
}

int	coder_acquire_dongles(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	if (simulation->stop_flag)
	{
		pthread_mutex_unlock(&simulation->sync.mutex);
		return (0);
	}
	scheduler_submit(simulation, coder);
	scheduler_grant(simulation);
	if (!wait_until_granted(coder))
	{
		pthread_mutex_unlock(&simulation->sync.mutex);
		return (0);
	}
	coder->granted = 0;
	pthread_mutex_unlock(&simulation->sync.mutex);
	log_dongles_taken(coder);
	return (1);
}

void	coder_release_dongles(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	dongles_release_both(coder->left_dongle, coder->right_dongle,
		simulation->configuration->dongle_cooldown);
	scheduler_grant(simulation);
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}
