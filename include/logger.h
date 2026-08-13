/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:48:54 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:48:58 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
