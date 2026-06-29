/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_codexion.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 00:00:20 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/06/28 00:00:27 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CODEXION_H
# define FT_CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# define MAX_VAL 2147483647

typedef struct s_input
{
	int		codes[7];
	char	scheduler[5];
}			t_input;

typedef struct s_priority_queue
{
	size_t	capacity;
	size_t	len;
	int		*pq;
}			t_pq;

char		*ft_itoa(int num);
void		ft_swap(void *a, void *b);
#endif
