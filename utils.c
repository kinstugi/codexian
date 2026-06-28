/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 01:50:31 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/06/28 01:50:34 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	ft_rev_str(char *str, int l, int r)
{
	char	tmp;

	while (l < r)
	{
		tmp = str[l];
		str[l] = str[r];
		str[r] = tmp;
		l++;
		r--;
	}
}

char	*ft_itoa(int num)
{
	char		*ret;
	int			i;
	long long	nbr;

	ret = malloc(12);
	if (!ret)
		return (0);
	i = 0;
	nbr = num;
	if (num < 0)
	{
		nbr *= -1;
		ret[i++] = '-';
	}
	while (nbr > 10)
	{
		ret[i++] = '0' + (nbr % 10);
		nbr /= 10;
	}
	ret[i++] = '0' + nbr;
	ft_rev_str(ret, 0 + (num < 0), i - 1);
	ret[i] = 0;
	return (ret);
}
