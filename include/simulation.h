#ifndef SIMULATION_H
# define SIMULATION_H

# include "types.h"

t_simulation	*simulation_init(t_config *config);
void			simulation_destroy(t_simulation *simulation);

#endif
