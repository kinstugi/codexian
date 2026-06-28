/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 00:00:56 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/06/28 00:00:59 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	main(int ac, char **av)
{
	t_input	app_input;
	int		i;
	char	*val;

	i = 0;
	if (ac == 9)
	{
		while (++i < 8)
		{
			app_input.codes[i - 1] = atoi(av[i]);
			val = ft_itoa(app_input.codes[i - 1]);
			if (strcmp(val, av[i]) != 0 || app_input.codes[i - 1] < 0)
			{
				free(val);
				break ;
			}
			free(val);
		}
	}
	return (0);
}
