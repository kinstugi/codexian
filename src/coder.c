/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
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
#include "simulation.h"
#include <pthread.h>

static void	log_dongles_taken(t_coder *coder)
{
	logger(coder->simulation, coder->id, "has taken a dongle");
	if (coder->left_dongle != coder->right_dongle)
		logger(coder->simulation, coder->id, "has taken a dongle");
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

static void	wait_for_dongles(t_coder *coder)
{
	t_simulation	*simulation;
	struct timespec	deadline;

	simulation = coder->simulation;
	ms_to_timespec(wait_deadline_ms(coder), &deadline);
	pthread_mutex_lock(&simulation->sync.mutex);
	if (!simulation->stop_flag)
		pthread_cond_timedwait(&simulation->sync.condition,
			&simulation->sync.mutex, &deadline);
	pthread_mutex_unlock(&simulation->sync.mutex);
}

static int	coder_acquire_dongles(t_coder *coder)
{
	while (1)
	{
		if (simulation_stopped(coder->simulation))
			return (0);
		if (dongles_try_acquire_both(coder->left_dongle,
				coder->right_dongle, coder->id))
		{
			log_dongles_taken(coder);
			return (1);
		}
		wait_for_dongles(coder);
	}
}

static void	coder_release_dongles(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	dongles_release_both(coder->left_dongle, coder->right_dongle,
		simulation->configuration->dongle_cooldown);
	pthread_mutex_lock(&simulation->sync.mutex);
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}

static void	compile_cycle(t_coder *coder)
{
	t_simulation	*simulation;
	t_config		*config;

	simulation = coder->simulation;
	config = simulation->configuration;
	coder->state = COMPILING;
	coder->last_compile_start = get_current_time_ms();
	coder->compile_count += 1;
	logger(simulation, coder->id, "is compiling");
	sleep_ms(config->time_to_compile);
	coder_release_dongles(coder);
	coder->state = DEBUGGING;
	logger(simulation, coder->id, "is debugging");
	sleep_ms(config->time_to_debug);
	coder->state = REFACTORING;
	logger(simulation, coder->id, "is refactoring");
	sleep_ms(config->time_to_refactor);
}

void	*coder_routine(void *arg)
{
	t_coder			*coder;
	t_simulation	*simulation;
	t_config		*config;

	coder = (t_coder *)arg;
	simulation = coder->simulation;
	config = simulation->configuration;
	while (!simulation_stopped(simulation)
		&& coder->compile_count < config->number_of_compiles_required)
	{
		coder->state = WAITING;
		if (!coder_acquire_dongles(coder))
			break ;
		compile_cycle(coder);
	}
	coder->state = DONE;
	return (NULL);
}
