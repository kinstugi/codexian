#include "heap.h"
#include <stdlib.h>

static void	heap_swap(t_heap_item *a, t_heap_item *b)
{
	t_heap_item	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static int	item_before(t_heap_item a, t_heap_item b)
{
	if (a.key < b.key)
		return (1);
	if (a.key == b.key)
		return (a.tie < b.tie);
	return (0);
}

int	heap_init(t_heap *heap, int capacity)
{
	heap->items = NULL;
	heap->capacity = capacity;
	heap->size = 0;
	if (capacity <= 0)
		return (0);
	heap->items = malloc(sizeof(t_heap_item) * capacity);
	if (!heap->items)
		return (0);
	return (1);
}

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
		if (!item_before(heap->items[child], heap->items[parent]))
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
		if (left < heap->size && item_before(heap->items[left],
				heap->items[child]))
			child = left;
		if (right < heap->size && item_before(heap->items[right],
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

void	*heap_peek(t_heap *heap)
{
	if (heap->size == 0)
		return (NULL);
	return (heap->items[0].data);
}

int	heap_empty(t_heap *heap)
{
	return (heap->size == 0);
}

int	heap_size(t_heap *heap)
{
	return (heap->size);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->items);
	heap->items = NULL;
	heap->capacity = 0;
	heap->size = 0;
}
