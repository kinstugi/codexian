#ifndef HEAP_H
# define HEAP_H

# include "types.h"

int		heap_init(t_heap *heap, int capacity);
int		heap_push(t_heap *heap, long key, long tie, void *data);
void	*heap_pop(t_heap *heap);
void	*heap_peek(t_heap *heap);
int		heap_empty(t_heap *heap);
int		heap_size(t_heap *heap);
void	heap_destroy(t_heap *heap);

#endif
