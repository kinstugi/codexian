#include "parser.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int	parse_number(const char *str, int *out)
{
	char	*end;
	long	value;

	errno = 0;
	value = strtol(str, &end, 10);
	if (str == end || *end != '\0' || errno == ERANGE)
		return (0);
	if (value < 0 || value > INT_MAX)
		return (0);
	*out = (int)value;
	return (1);
}

static int	parse_scheduler(const char *str, sched_type *out)
{
	if (strcmp(str, "fifo") == 0)
		*out = FIFO;
	else if (strcmp(str, "edf") == 0)
		*out = EDF;
	else
		return (0);
	return (1);
}

static int	validate_numbers(t_config *config, char **av)
{
	if (!parse_number(av[1], &config->number_of_coders)
		|| config->number_of_coders == 0)
		return (0);
	if (!parse_number(av[2], &config->time_to_burnout)
		|| !parse_number(av[3], &config->time_to_compile)
		|| !parse_number(av[4], &config->time_to_debug)
		|| !parse_number(av[5], &config->time_to_refactor)
		|| !parse_number(av[6], &config->number_of_compiles_required)
		|| !parse_number(av[7], &config->dongle_cooldown))
		return (0);
	return (1);
}

static t_config	*attach_scheduler(t_config *config, char *str)
{
	if (parse_scheduler(str, &config->scheduler))
		return (config);
	fprintf(stderr, "codexion: invalid scheduler (use 'fifo' or 'edf')\n");
	free(config);
	return (0);
}

t_config	*parse_args(int ac, char **av)
{
	t_config	*config;

	if (ac != 9)
	{
		fprintf(stderr, "codexion: wrong argument count (expected 8)\n");
		return (0);
	}
	config = malloc(sizeof(t_config));
	if (!config)
		return (0);
	if (!validate_numbers(config, av))
	{
		fprintf(stderr, "codexion: invalid numeric argument\n");
		free(config);
		return (0);
	}
	return (attach_scheduler(config, av[8]));
}
