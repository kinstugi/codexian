/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_destroy.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:44:05 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:44:08 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongle.h"
#include "scheduler.h"
#include "simulation.h"
#include "simulation_internal.h"
#include <pthread.h>
#include <stdlib.h>

static void	destroy_threads(t_simulation *simulation)
{
	if (simulation->monitor_started)
	{
		simulation_stop(simulation);
		pthread_join(simulation->monitor, NULL);
		simulation->monitor_started = 0;
	}
	if (simulation->coders_started > 0)
	{
		simulation_stop(simulation);
		join_coders(simulation, simulation->coders_started);
	}
}

static void	destroy_dongles_ready(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->dongles_ready)
	{
		dongle_destroy(&simulation->dongles[i]);
		i++;
	}
}

static void	destroy_sync(t_simulation *simulation)
{
	if (simulation->sync_cond_ready)
		pthread_cond_destroy(&simulation->sync.condition);
	if (simulation->sync_mutex_ready)
		pthread_mutex_destroy(&simulation->sync.mutex);
	if (simulation->logging_ready)
		pthread_mutex_destroy(&simulation->logging);
}

void	simulation_destroy(t_simulation *simulation)
{
	destroy_threads(simulation);
	destroy_dongles_ready(simulation);
	if (simulation->scheduler_ready)
		scheduler_destroy(&simulation->scheduler);
	destroy_sync(simulation);
	free(simulation->coders);
	free(simulation->dongles);
	free(simulation->configuration);
	free(simulation);
}
