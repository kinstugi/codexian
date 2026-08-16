/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:39:22 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:39:28 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"
#include "logger.h"
#include <pthread.h>

void	coder_set_state(t_coder *coder, t_coder_state state)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	if (coder->state != BURNED_OUT)
		coder->state = state;
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}

int	coder_should_run(t_coder *coder)
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

int	coder_begin_compile(t_coder *coder)
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

void	coder_complete_compile(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	if (coder->state != BURNED_OUT)
		coder->compile_count += 1;
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}

void	coder_finish(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->sync.mutex);
	if (coder->state != BURNED_OUT)
		coder->state = DONE;
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}
