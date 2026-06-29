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

void		ft_swap(int *a, int *b);
char		*ft_strcpy(char *dest, char *src, size_t len);
int			ft_valid_num(char *num_str);
int			min_pq_init(t_pq *min_q, size_t size);
int			min_q_pop_top(t_pq *q);
int			min_q_push(t_pq *mq, int num);
void		min_q_clear(t_pq *mq);
#endif
