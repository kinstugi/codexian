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
#include "logger.h"

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
