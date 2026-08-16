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
#include "scheduler.h"
#include "simulation.h"
#include <pthread.h>

static void	log_dongles_taken(t_coder *coder)
{
	logger(coder->simulation, coder->id, "has taken a dongle");
	if (coder->left_dongle != coder->right_dongle)
		logger(coder->simulation, coder->id, "has taken a dongle");
}

static void	coder_set_state(t_coder *coder, t_coder_state state)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	if (coder->state != BURNED_OUT)
		coder->state = state;
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}

static int	coder_should_run(t_coder *coder)
{
	t_simulation	*simulation;
	int			running;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	running = (!simulation->stop_flag && coder->compile_count
		< simulation->configuration->number_of_compiles_required);
	pthread_mutex_unlock(&simulation->sync.mutex);
	return (running);
}

static int	coder_wait_ms(t_simulation *simulation, long ms)
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

static int	coder_begin_compile(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	if (simulation->stop_flag)
	{
		pthread_mutex_unlock(&simulation->sync.mutex);
		return (0);
	}
	coder->state = COMPILING;
	coder->last_compile_start = get_current_time_ms();
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
	logger(simulation, coder->id, "is compiling");
	return (1);
}

static void	coder_complete_compile(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	if (coder->state != BURNED_OUT)
		coder->compile_count += 1;
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
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

static int	wait_until_granted(t_coder *coder)
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

static int	coder_acquire_dongles(t_coder *coder)
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

static void	coder_release_dongles(t_coder *coder)
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

static int	debug_and_refactor(t_coder *coder)
{
	t_simulation	*simulation;
	t_config		*config;

	simulation = coder->simulation;
	config = simulation->configuration;
	coder_set_state(coder, DEBUGGING);
	logger(simulation, coder->id, "is debugging");
	if (!coder_wait_ms(simulation, config->time_to_debug))
		return (0);
	coder_set_state(coder, REFACTORING);
	logger(simulation, coder->id, "is refactoring");
	if (!coder_wait_ms(simulation, config->time_to_refactor))
		return (0);
	return (1);
}

static int	compile_cycle(t_coder *coder)
{
	t_simulation	*simulation;
	t_config		*config;

	simulation = coder->simulation;
	config = simulation->configuration;
	if (!coder_begin_compile(coder))
	{
		coder_release_dongles(coder);
		return (0);
	}
	if (!coder_wait_ms(simulation, config->time_to_compile))
	{
		coder_release_dongles(coder);
		return (0);
	}
	coder_complete_compile(coder);
	coder_release_dongles(coder);
	if (!coder_should_run(coder))
		return (0);
	return (debug_and_refactor(coder));
}

static void	coder_finish(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	if (coder->state != BURNED_OUT)
		coder->state = DONE;
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (coder_should_run(coder))
	{
		coder_set_state(coder, WAITING);
		if (!coder_acquire_dongles(coder))
			break ;
		if (!compile_cycle(coder))
			break ;
	}
	coder_finish(coder);
	return (NULL);
}
