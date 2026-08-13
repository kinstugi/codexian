/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:42:15 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:42:19 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>

static void	print_usage(void)
{
	fprintf(stderr,
			"usage: ./codexion number_of_coders time_to_burnout "
			"time_to_compile time_to_debug time_to_refactor "
			"number_of_compiles_required dongle_cooldown scheduler\n");
	fprintf(stderr, "       scheduler: 'fifo' or 'edf'\n");
}

static void	print_config_accepted(t_config *config)
{
	char	*scheduler;

	if (config->scheduler == FIFO)
		scheduler = "fifo";
	else
		scheduler = "edf";
	fprintf(stderr,
			"codexion: config accepted: %d coders, %dms burnout, "
			"%dms compile, %dms debug, %dms refactor, %d compiles, "
			"%dms cooldown, scheduler=%s\n",
			config->number_of_coders,
			config->time_to_burnout,
			config->time_to_compile,
			config->time_to_debug,
			config->time_to_refactor,
			config->number_of_compiles_required,
			config->dongle_cooldown,
			scheduler);
}

int	main(int argc, char **argv)
{
	t_config		*config;
	t_simulation	*simulation;

	config = parse_args(argc, argv);
	if (!config)
	{
		print_usage();
		return (1);
	}
	print_config_accepted(config);
	simulation = simulation_init(config);
	if (!simulation)
	{
		fprintf(stderr, "codexion: failed to initialize simulation\n");
		return (1);
	}
	simulation_destroy(simulation);
	return (0);
}
