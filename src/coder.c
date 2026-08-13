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

static void	release_dongles(t_coder *coder)
{
	dongle_release(coder->left_dongle);
	if (coder->left_dongle != coder->right_dongle)
		dongle_release(coder->right_dongle);
}

static int	take_dongles(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	if (coder->left_dongle == coder->right_dongle)
	{
		if (!dongle_acquire(coder->left_dongle, coder->id))
			return (0);
	}
	else
	{
		if (!dongle_acquire(coder->left_dongle, coder->id))
			return (0);
		if (!dongle_acquire(coder->right_dongle, coder->id))
		{
			dongle_release(coder->left_dongle);
			return (0);
		}
	}
	logger(simulation, coder->id, "has taken a dongle");
	if (coder->left_dongle != coder->right_dongle)
		logger(simulation, coder->id, "has taken a dongle");
	return (1);
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
	release_dongles(coder);
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
		if (!take_dongles(coder))
			continue ;
		compile_cycle(coder);
	}
	coder->state = DONE;
	return (NULL);
}
