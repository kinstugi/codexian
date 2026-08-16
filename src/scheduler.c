/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:43:53 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:43:57 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scheduler.h"
#include "dongle.h"
#include "heap.h"
#include "logger.h"
#include <pthread.h>
#include <stdlib.h>

static long	request_key(t_scheduler *sched, t_coder *coder)
{
	if (sched->type == FIFO)
		return (coder->request.seq);
	return (coder->request.deadline);
}

static long	request_tie(t_coder *coder)
{
	return (coder->request.seq);
}

int	scheduler_init(t_scheduler *sched, int n, t_sched_type type)
{
	sched->next_seq = 0;
	sched->type = type;
	sched->pending = NULL;
	sched->heap.items = NULL;
	sched->heap.size = 0;
	sched->heap.capacity = 0;
	sched->pending = malloc(sizeof(t_coder *) * n);
	if (!sched->pending)
		return (0);
	if (!heap_init(&sched->heap, n))
	{
		free(sched->pending);
		sched->pending = NULL;
		return (0);
	}
	return (1);
}

void	scheduler_destroy(t_scheduler *sched)
{
	heap_destroy(&sched->heap);
	free(sched->pending);
	sched->pending = NULL;
}

void	scheduler_submit(t_simulation *sim, t_coder *coder)
{
	t_scheduler	*sched;

	sched = &sim->scheduler;
	coder->request.coder = coder;
	coder->request.seq = sched->next_seq;
	sched->next_seq += 1;
	coder->request.timestamp = get_current_time_ms();
	coder->request.deadline = coder->last_compile_start
		+ sim->configuration->time_to_burnout;
	coder->granted = 0;
	heap_push(&sched->heap, request_key(sched, coder),
		request_tie(coder), coder);
}

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
			heap_push(&sched->heap, request_key(sched, coder),
				request_tie(coder), coder);
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
