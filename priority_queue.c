/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   priority_queue.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 02:56:20 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/06/29 18:01:39 by kwaku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	min_pq_init(t_pq *min_q, size_t size)
{
	size_t	i;

	i = 0;
	min_q->pq = malloc(sizeof(int) * (size + 1));
	if (!min_q->pq)
		return (0);
	min_q->capacity = size;
	min_q->len = 0;
	while (i <= size)
	{
		min_q->pq[i] = MAX_VAL;
		i++;
	}
	return (1);
}

int	min_q_pop_top(t_pq *q)
{
	int		ret;
	size_t	i;

	i = 0;
	ret = q->pq[0];
	ft_swap(&(q->pq[0]), &(q->pq[q->len - 1]));
	q->pq[q->len - 1] = MAX_VAL;
	q->len--;
	while (2 * i + 1 < q->len)
	{
		if (q->pq[2 * i + 1] < q->pq[2 * i + 2] && q->pq[2 * i + 1] < q->pq[i])
		{
			ft_swap(&(q->pq[2 * i + 1]), &(q->pq[i]));
			i = 2 * i + 1;
		}
		else if (q->pq[i] > q->pq[2 * i + 2])
		{
			ft_swap(&(q->pq[2 * i + 2]), &(q->pq[i]));
			i = 2 * i + 2;
		}
		else
			break ;
	}
	return (ret);
}

int	min_q_push(t_pq *mq, int num)
{
	int	child;
	int	parent;

	mq->pq[mq->len] = num;
	mq->len++;
	child = mq->len - 1;
	while (child > 0)
	{
		parent = (child - 2 + (child % 2)) / 2;
		if (mq->pq[child] < mq->pq[parent])
			ft_swap(&(mq->pq[child]), &(mq->pq[parent]));
		child = parent;
	}
	return (0);
}

void	min_q_clear(t_pq *mq)
{
	mq->len = 0;
	mq->capacity = 0;
	free(mq->pq);
	mq->pq = 0;
}
