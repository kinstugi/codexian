/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_grant.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:43:53 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:43:57 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongle.h"
#include "heap.h"
#include "scheduler.h"
#include "scheduler_internal.h"
#include <pthread.h>

static int	coder_grantable(t_coder *coder)
{
	if (!dongle_is_free(coder->left_dongle))
		return (0);
	if (coder->left_dongle != coder->right_dongle
		&& !dongle_is_free(coder->right_dongle))
		return (0);
	return (1);
}

static void	grant_coder(t_coder *coder)
{
	dongle_claim(coder->left_dongle, coder->id);
	if (coder->left_dongle != coder->right_dongle)
		dongle_claim(coder->right_dongle, coder->id);
	coder->granted = 1;
}

static int	grant_pass(t_simulation *sim, int count)
{
	t_scheduler	*sched;
	t_coder		*coder;
	int			i;
	int			granted;

	sched = &sim->scheduler;
	granted = 0;
	i = 0;
	while (i < count)
	{
		coder = sched->pending[i];
		if (coder_grantable(coder))
		{
			grant_coder(coder);
			granted = 1;
		}
		else
			heap_push(&sched->heap, scheduler_request_key(sched, coder),
				scheduler_request_tie(coder), coder);
		i++;
	}
	return (granted);
}

void	scheduler_grant(t_simulation *sim)
{
	t_scheduler	*sched;
	int			count;

	sched = &sim->scheduler;
	if (sim->stop_flag)
		return ;
	count = 0;
	while (!heap_empty(&sched->heap))
		sched->pending[count++] = (t_coder *)heap_pop(&sched->heap);
	if (grant_pass(sim, count))
		pthread_cond_broadcast(&sim->sync.condition);
}
