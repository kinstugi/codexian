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
		if (input->codes[i - 1] < 0)
		{
			flag = 0;
			break ;
		}
	}
	ft_strcpy(input->scheduler, av[8], strlen(av[8]));
	if (strcmp("fifo", av[8]) && strcmp("edf", av[8]))
		flag = 0;
	return (flag);
}

int	main(int ac, char **av)
{
	t_pq	mq;
	int		num;

	// t_input	app_input;
	// process_input(ac, av, &app_input);
	min_pq_init(&mq, ac - 1);
	for (int i = 1; i < ac; i++)
	{
		num = atoi(av[i]);
		min_q_push(&mq, num);
	}
	while (mq.len)
	{
		num = min_q_pop_top(&mq);
		printf("%d ", num);
	}
	printf("\n");
	min_q_clear(&mq);
	return (0);
}
