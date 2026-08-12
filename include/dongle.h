#ifndef DONGLE_H
# define DONGLE_H

/*
** Dongle resource model, mutex ownership, and cooldown.
** Implemented in milestone M4.
*/

# include "types.h"

int		dongle_init(t_dongle *dongle, int id);
void	dongle_destroy(t_dongle *dongle);
int		dongle_can_be_acquired(t_dongle *dongle);
int		dongle_release(t_dongle *dongle);

#endif
