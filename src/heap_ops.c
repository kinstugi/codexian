/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:40:08 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:40:19 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"
#include "heap_internal.h"

int	heap_push(t_heap *heap, long key, long tie, void *data)
{
	int	child;
	int	parent;

	if (heap->size >= heap->capacity)
		return (0);
	child = heap->size;
	heap->items[child].key = key;
	heap->items[child].tie = tie;
	heap->items[child].data = data;
	heap->size += 1;
	while (child > 0)
	{
		parent = (child - 1) / 2;
		if (!heap_item_before(heap->items[child], heap->items[parent]))
			break ;
		heap_swap(&heap->items[child], &heap->items[parent]);
		child = parent;
	}
	return (1);
}

static void	heap_sift_down(t_heap *heap, int root)
{
	int	child;
	int	left;
	int	right;

	while (root < heap->size)
	{
		left = root * 2 + 1;
		right = root * 2 + 2;
		child = root;
		if (left < heap->size && heap_item_before(heap->items[left],
				heap->items[child]))
			child = left;
		if (right < heap->size && heap_item_before(heap->items[right],
				heap->items[child]))
			child = right;
		if (child == root)
			break ;
		heap_swap(&heap->items[root], &heap->items[child]);
		root = child;
	}
}

void	*heap_pop(t_heap *heap)
{
	void	*data;

	if (heap->size == 0)
		return (NULL);
	data = heap->items[0].data;
	heap->size -= 1;
	heap->items[0] = heap->items[heap->size];
	heap_sift_down(heap, 0);
	return (data);
}
