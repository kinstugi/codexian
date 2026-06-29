/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 01:50:31 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/06/29 01:41:37 by kwaku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	ft_valid_num(char *num_str)
{
	int			i;
	long long	num;
	int			d_cnt;

	d_cnt = 0;
	i = 0;
	num = 0;
	while (num_str[i] && num_str[i] == ' ')
		i++;
	if (num_str[i] == '-')
		return (-1);
	i = i + (num_str[i] == '+');
	while (num_str[i] && ('0' <= num_str[i] && num_str[i] <= '9'))
	{
		num = (num * 10) + num_str[i] - '0';
		if (num > MAX_VAL || d_cnt > 11)
			return (-1);
		i++;
		d_cnt++;
	}
	while (num_str[i] && num_str[i] == ' ')
		i++;
	if (num_str[i] != 0 || d_cnt == 0)
		return (-1);
	return ((int)num);
}

void	ft_swap(int *a, int *b)
{
	int	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

char	*ft_strcpy(char *dest, char *src, size_t len)
{
	size_t	i;

	if (len > 4)
		len = 4;
	i = 0;
	while (i < len)
	{
		dest[i] = src[i];
		i++;
	}
	dest[len] = 0;
	return (dest);
}
