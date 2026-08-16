/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:40:08 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:40:19 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap_internal.h"

void	heap_swap(t_heap_item *a, t_heap_item *b)
{
	t_heap_item	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int	heap_item_before(t_heap_item a, t_heap_item b)
{
	if (a.key < b.key)
		return (1);
	if (a.key == b.key)
		return (a.tie < b.tie);
	return (0);
}
