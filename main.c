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

int	process_input(int ac, char **av, t_input *input)
{
	char	*num_str;
	int		flag;
	int		i;

	flag = 1;
	if (ac != 9)
		return (0);
	i = 0;
	while (++i < 8)
	{
		input->codes[i - 1] = ft_valid_num(av[i]);
		if (input->codes[i-1] < 0)
		{
			flag = 0;
			break;
		}
	}
	ft_strcpy(input->scheduler, av[8], strlen(av[8]));
	if (strcmp("fifo", av[8]) && strcmp("edf", av[8]))
		flag = 0;
	return (flag);
}

int	main(int ac, char **av)
{
	t_input	app_input;

	return (0);
}
