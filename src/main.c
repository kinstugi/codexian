#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

static void	print_usage(void)
{
	fprintf(stderr, "usage: ./codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler\n");
	fprintf(stderr, "       scheduler: 'fifo' or 'edf'\n");
}

int	main(int argc, char **argv)
{
	t_config	*config;

	config = parse_args(argc, argv);
	if (!config)
	{
		print_usage();
		return (1);
	}
	printf("codexion: config accepted: %d coders, %dms burnout, "
		"%dms compile, %dms debug, %dms refactor, %d compiles, "
		"%dms cooldown, scheduler=%s\n",
		config->number_of_coders,
		config->time_to_burnout,
		config->time_to_compile,
		config->time_to_debug,
		config->time_to_refactor,
		config->number_of_compiles_required,
		config->dongle_cooldown,
		config->scheduler == FIFO ? "fifo" : "edf");
	free(config);
	return (0);
}
