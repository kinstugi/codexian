/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:40:44 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:40:48 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongle.h"
#include <pthread.h>

int	dongle_init(t_dongle *dongle, int id)
{
	dongle->id = id;
	dongle->available_at = 0;
	dongle->owner = -1;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (0);
	return (1);
}

void	dongle_destroy(t_dongle *dongle)
{
	pthread_mutex_destroy(&dongle->mutex);
}

int	dongle_can_be_acquired(t_dongle *dongle)
{
	int	can_acquire;

	pthread_mutex_lock(&dongle->mutex);
	can_acquire = (dongle->owner == -1);
	pthread_mutex_unlock(&dongle->mutex);
	return (can_acquire);
}

int	dongle_acquire(t_dongle *dongle, int coder_id)
{
	int	acquired;

	acquired = 0;
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner == -1)
	{
		dongle->owner = coder_id;
		acquired = 1;
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (acquired);
}

int	dongle_release(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->owner = -1;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}
