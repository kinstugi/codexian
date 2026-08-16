/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:42:44 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:42:59 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"
#include "monitor.h"

static int	coder_missed_deadline(t_simulation *sim, t_coder *coder, long now)
{
	long	delta;

	if (coder->state == DONE || coder->state == COMPILING)
		return (0);
	if (coder->compile_count >= sim->configuration->number_of_compiles_required)
		return (0);
	delta = now - coder->last_compile_start;
	return (delta >= sim->configuration->time_to_burnout);
}

static int	monitor_check_burnout(t_simulation *sim, long now)
{
	int	i;
	int	n;

	n = sim->configuration->number_of_coders;
	i = 0;
	while (i < n)
	{
		if (coder_missed_deadline(sim, &sim->coders[i], now))
		{
			sim->coders[i].state = BURNED_OUT;
			sim->stop_reason = 1;
			sim->stop_flag = 1;
			pthread_cond_broadcast(&sim->sync.condition);
			logger_force(sim, sim->coders[i].id, "burned out");
			return (1);
		}
		i++;
	}
	return (0);
}

static int	monitor_all_done(t_simulation *sim)
{
	int	i;
	int	n;
	int	required;

	n = sim->configuration->number_of_coders;
	required = sim->configuration->number_of_compiles_required;
	i = 0;
	while (i < n)
	{
		if (sim->coders[i].compile_count < required)
			return (0);
		i++;
	}
	sim->stop_reason = 2;
	sim->stop_flag = 1;
	pthread_cond_broadcast(&sim->sync.condition);
	return (1);
}

static long	monitor_next_deadline(t_simulation *sim, long now)
{
	long	deadline;
	long	candidate;
	int		i;

	deadline = now + 10;
	i = 0;
	while (i < sim->configuration->number_of_coders)
	{
		if (sim->coders[i].state != DONE
			&& sim->coders[i].state != COMPILING)
		{
			candidate = sim->coders[i].last_compile_start
				+ sim->configuration->time_to_burnout;
			if (candidate < deadline)
				deadline = candidate;
		}
		i++;
	}
	return (deadline);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	struct timespec	deadline;
	long			now;

	sim = (t_simulation *)arg;
	pthread_mutex_lock(&sim->sync.mutex);
	while (!sim->stop_flag)
	{
		now = get_current_time_ms();
		if (monitor_check_burnout(sim, now) || monitor_all_done(sim))
			break ;
		ms_to_timespec(monitor_next_deadline(sim, now), &deadline);
		pthread_cond_timedwait(&sim->sync.condition,
			&sim->sync.mutex, &deadline);
	}
	pthread_mutex_unlock(&sim->sync.mutex);
	return (NULL);
}
