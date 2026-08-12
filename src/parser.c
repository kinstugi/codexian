#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char	*ft_itoa(int n)
{
	static char		buf[12];
	char			*tmp;
	unsigned int	value;

	tmp = buf + sizeof(buf) - 1;
	*tmp = '\0';
	value = (unsigned int)n;
	if (n < 0)
		value = 0U - value;
	while (value != 0)
	{
		*--tmp = '0' + (value % 10);
		value /= 10;
	}
	if (n == 0)
		*--tmp = '0';
	if (n < 0)
		*--tmp = '-';
	return (tmp);
}

static int	is_valid_number(const char *str, int *out)
{
	int	value;

	value = atoi(str);
	if (value < 0)
		return (0);
	if (strcmp(ft_itoa(value), str) != 0)
		return (0);
	*out = value;
	return (1);
}

static int	validate_numbers(t_config *config, char **av)
{
	if (!is_valid_number(av[1], &config->number_of_coders)
		|| config->number_of_coders == 0)
	{
		fprintf(stderr, "codexion: invalid numeric argument\n");
		return (0);
	}
	if (!is_valid_number(av[2], &config->time_to_burnout)
		|| !is_valid_number(av[3], &config->time_to_compile)
		|| !is_valid_number(av[4], &config->time_to_debug)
		|| !is_valid_number(av[5], &config->time_to_refactor)
		|| !is_valid_number(av[6], &config->number_of_compiles_required)
		|| !is_valid_number(av[7], &config->dongle_cooldown))
	{
		fprintf(stderr, "codexion: invalid numeric argument\n");
		return (0);
	}
	return (1);
}

static int	set_scheduler(t_config *config, char *str)
{
	if (strcmp(str, "fifo") == 0)
		config->scheduler = FIFO;
	else if (strcmp(str, "edf") == 0)
		config->scheduler = EDF;
	else
	{
		fprintf(stderr, "codexion: invalid scheduler (use fifo or edf)\n");
		return (0);
	}
	return (1);
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
	if (!validate_numbers(config, av) || !set_scheduler(config, av[8]))
	{
		free(config);
		return (0);
	}
	return (config);
}
