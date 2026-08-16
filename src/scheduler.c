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

#include "heap.h"
#include "logger.h"
#include "scheduler.h"
#include "scheduler_internal.h"
#include <stdlib.h>

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
	heap_push(&sched->heap, scheduler_request_key(sched, coder),
		scheduler_request_tie(coder), coder);
}
