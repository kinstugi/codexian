/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_internal.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:40:08 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:40:19 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEAP_INTERNAL_H
# define HEAP_INTERNAL_H

# include "types.h"

void	heap_swap(t_heap_item *a, t_heap_item *b);
int		heap_item_before(t_heap_item a, t_heap_item b);

#endif
