/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_run.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:44:05 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:44:08 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"
#include "logger.h"
#include "monitor.h"
#include "simulation.h"
#include "simulation_internal.h"
#include <pthread.h>

void	join_coders(t_simulation *simulation, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(simulation->coders[i].thread, NULL);
		i++;
	}
	if (simulation->coders_started >= count)
		simulation->coders_started -= count;
	else
		simulation->coders_started = 0;
}

static int	start_monitor(t_simulation *simulation)
{
	if (pthread_create(&simulation->monitor, NULL,
			monitor_routine, simulation) != 0)
	{
		simulation_stop(simulation);
		return (0);
	}
	simulation->monitor_started = 1;
	return (1);
}

static int	start_coders(t_simulation *simulation)
{
	int	i;
	int	n;

	n = simulation->configuration->number_of_coders;
	i = 0;
	while (i < n)
	{
		simulation->coders[i].last_compile_start = simulation->start_time;
		if (pthread_create(&simulation->coders[i].thread, NULL,
				coder_routine, &simulation->coders[i]) != 0)
		{
			simulation_stop(simulation);
			join_coders(simulation, i);
			return (0);
		}
		simulation->coders_started += 1;
		i++;
	}
	return (1);
}

int	simulation_run(t_simulation *simulation)
{
	int	n;

	n = simulation->configuration->number_of_coders;
	simulation->start_time = get_current_time_ms();
	if (!start_coders(simulation))
		return (0);
	if (!start_monitor(simulation))
	{
		join_coders(simulation, n);
		return (0);
	}
	pthread_join(simulation->monitor, NULL);
	simulation->monitor_started = 0;
	join_coders(simulation, n);
	return (1);
}
