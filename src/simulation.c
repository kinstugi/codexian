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

#include "simulation.h"
#include <pthread.h>

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
	if (!simulation->sync_mutex_ready)
		return ;
	pthread_mutex_lock(&simulation->sync.mutex);
	simulation->stop_flag = 1;
	if (simulation->sync_cond_ready)
		pthread_cond_broadcast(&simulation->sync.condition);
	pthread_mutex_unlock(&simulation->sync.mutex);
}
