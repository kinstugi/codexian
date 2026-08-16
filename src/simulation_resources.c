/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_resources.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:44:05 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:44:08 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongle.h"
#include "simulation_internal.h"
#include <stdlib.h>

static void	destroy_dongles(t_simulation *simulation, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		dongle_destroy(&simulation->dongles[i]);
		i++;
	}
}

int	init_dongles(t_simulation *simulation)
{
	int	n;
	int	i;

	n = simulation->configuration->number_of_coders;
	simulation->dongles = malloc(sizeof(t_dongle) * n);
	if (!simulation->dongles)
		return (0);
	i = 0;
	while (i < n)
	{
		if (dongle_init(&simulation->dongles[i], i) == 0)
		{
			destroy_dongles(simulation, i);
			free(simulation->dongles);
			simulation->dongles = NULL;
			return (0);
		}
		i++;
	}
	return (1);
}

static int	assign_topology(t_simulation *simulation)
{
	int	n;
	int	i;

	n = simulation->configuration->number_of_coders;
	i = 0;
	while (i < n)
	{
		simulation->coders[i].id = i + 1;
		simulation->coders[i].left_dongle = &simulation->dongles[i];
		simulation->coders[i].right_dongle = &simulation->dongles[(i + 1) % n];
		simulation->coders[i].compile_count = 0;
		simulation->coders[i].last_compile_start = 0;
		simulation->coders[i].state = WAITING;
		simulation->coders[i].simulation = simulation;
		i++;
	}
	return (1);
}

int	init_coders(t_simulation *simulation)
{
	int	n;

	n = simulation->configuration->number_of_coders;
	simulation->coders = malloc(sizeof(t_coder) * n);
	if (!simulation->coders)
		return (0);
	return (assign_topology(simulation));
}
