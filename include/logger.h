#ifndef LOGGER_H
# define LOGGER_H

/*
** Serialized, timestamped log output.
** Implemented in milestone M3.
*/

# include "types.h"

long	get_current_time_ms(void);
long	get_elapsed_time_ms(t_simulation *simulation);
void	logger(t_simulation *simulation, int coder_id, const char *message);

#endif
