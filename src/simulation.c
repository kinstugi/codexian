/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
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
#include <pthread.h>
#include <stdlib.h>

int	simulation_stopped(t_simulation *simulation)
{
	int	stopped;

	pthread_mutex_lock(&simulation->sync.mutex);
	stopped = simulation->stop_flag;
	pthread_mutex_unlock(&simulation->sync.mutex);
	return (stopped);
}

void	simulation_stop(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->sync.mutex);
	simulation->stop_flag = 1;
	pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}

void	simulation_destroy(t_simulation *simulation)
{
	int	n;
	int	i;

	n = simulation->configuration->number_of_coders;
	i = 0;
	if (simulation->dongles)
	{
		while (i < n)
		{
			dongle_destroy(&simulation->dongles[i]);
			i++;
		}
	}
	pthread_mutex_destroy(&simulation->logging);
	pthread_mutex_destroy(&simulation->sync.mutex);
	pthread_cond_destroy(&simulation->sync.condition);
	scheduler_destroy(&simulation->scheduler);
	free(simulation->coders);
	free(simulation->dongles);
	free(simulation->configuration);
	free(simulation);
}
