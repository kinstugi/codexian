/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   priority_queue.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 02:56:20 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/06/29 02:09:16 by kwaku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	initialize_min_pq(t_pq *min_q, size_t size)
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

int	pop_top(t_pq *q)
{
	int		ret;
	size_t	i;

	i = 0;
	ret = q->pq[0];
	ft_swap(&(q->pq[0]), &(q->pq[q->len - 1]));
	q->len--;
	while (2 * i + 1 < q->len)
	{
	}
	return (ret);
}
