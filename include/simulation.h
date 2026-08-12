#ifndef SIMULATION_H
# define SIMULATION_H

/*
** Simulation lifecycle, central state, and shutdown.
** Implemented from milestone M4 onwards.
** simulation_init takes ownership of config (freed on destroy).
*/

# include "types.h"

t_simulation	*simulation_init(t_config *config);
void	simulation_destroy(t_simulation *simulation);

#endif
