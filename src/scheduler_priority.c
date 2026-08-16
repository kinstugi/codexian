/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_priority.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:43:53 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:43:57 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scheduler_internal.h"

long	scheduler_request_key(t_scheduler *sched, t_coder *coder)
{
	if (sched->type == FIFO)
		return (coder->request.seq);
	return (coder->request.deadline);
}

long	scheduler_request_tie(t_coder *coder)
{
	return (coder->request.seq);
}
