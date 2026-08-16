/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:44:05 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:44:08 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scheduler.h"
#include "simulation.h"
#include "simulation_internal.h"
#include <pthread.h>
#include <stdlib.h>

static t_simulation	*alloc_simulation(t_config *config)
{
	t_simulation	*simulation;

	simulation = malloc(sizeof(t_simulation));
	if (!simulation)
		return (NULL);
	simulation->configuration = config;
	simulation->dongles = NULL;
	simulation->coders = NULL;
	simulation->start_time = 0;
	simulation->stop_flag = 0;
	simulation->stop_reason = 0;
	simulation->scheduler.pending = NULL;
	simulation->scheduler.heap.items = NULL;
	simulation->scheduler.heap.size = 0;
	simulation->scheduler.heap.capacity = 0;
	simulation->scheduler.next_seq = 0;
	return (simulation);
}

static int	init_sync(t_simulation *simulation)
{
	if (pthread_mutex_init(&simulation->logging, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&simulation->sync.mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->logging);
		return (0);
	}
	if (pthread_cond_init(&simulation->sync.condition, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->sync.mutex);
		pthread_mutex_destroy(&simulation->logging);
		return (0);
	}
	return (1);
}

static int	init_scheduler(t_simulation *simulation)
{
	int	n;

	n = simulation->configuration->number_of_coders;
	return (scheduler_init(&simulation->scheduler, n,
			simulation->configuration->scheduler));
}

t_simulation	*simulation_init(t_config *config)
{
	t_simulation	*simulation;

	simulation = alloc_simulation(config);
	if (!simulation)
	{
		free(config);
		return (NULL);
	}
	if (init_sync(simulation) == 0)
	{
		free(simulation);
		free(config);
		return (NULL);
	}
	if (init_dongles(simulation) == 0 || init_coders(simulation) == 0
		|| init_scheduler(simulation) == 0)
	{
		simulation_destroy(simulation);
		return (NULL);
	}
	return (simulation);
}
