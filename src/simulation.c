#include "dongle.h"
#include "simulation.h"
#include <pthread.h>
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

static int	init_dongles(t_simulation *simulation)
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
		simulation->coders[i].right_dongle = \
			&simulation->dongles[(i + 1) % n];
		simulation->coders[i].compile_count = 0;
		simulation->coders[i].state = WAITING;
		simulation->coders[i].simulation = simulation;
		i++;
	}
	return (1);
}

static int	init_coders(t_simulation *simulation)
{
	int	n;

	n = simulation->configuration->number_of_coders;
	simulation->coders = malloc(sizeof(t_coder) * n);
	if (!simulation->coders)
		return (0);
	return (assign_topology(simulation));
}

static t_simulation	*alloc_simulation(t_config *config)
{
	t_simulation	*simulation;

	simulation = malloc(sizeof(t_simulation));
	if (!simulation)
		return (NULL);
	simulation->configuration = config;
	simulation->dongles = NULL;
	simulation->coders = NULL;
	return (simulation);
}

static int	init_sync(t_simulation *simulation)
{
	if (pthread_mutex_init(&simulation->logging, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&simulation->sync.mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->logging);
		return (0);
	}
	if (pthread_cond_init(&simulation->sync.condition, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->sync.mutex);
		pthread_mutex_destroy(&simulation->logging);
		return (0);
	}
	return (1);
}

t_simulation	*simulation_init(t_config *config)
{
	t_simulation	*simulation;

	simulation = alloc_simulation(config);
	if (!simulation)
	{
		free(config);
		return (NULL);
	}
	if (init_sync(simulation) == 0)
	{
		free(simulation);
		free(config);
		return (NULL);
	}
	if (init_dongles(simulation) == 0 || init_coders(simulation) == 0)
	{
		simulation_destroy(simulation);
		return (NULL);
	}
	return (simulation);
}

void	simulation_destroy(t_simulation *simulation)
{
	int	n;
	int	i;

	n = simulation->configuration->number_of_coders;
	i = 0;
	if (simulation->dongles)
	{
		while (i < n)
		{
			dongle_destroy(&simulation->dongles[i]);
			i++;
		}
	}
	pthread_mutex_destroy(&simulation->logging);
	pthread_mutex_destroy(&simulation->sync.mutex);
	pthread_cond_destroy(&simulation->sync.condition);
	free(simulation->coders);
	free(simulation->dongles);
	free(simulation->configuration);
	free(simulation);
}
