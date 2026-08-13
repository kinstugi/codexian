/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:49:54 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:49:58 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMULATION_H
# define SIMULATION_H

# include "types.h"

t_simulation	*simulation_init(t_config *config);
void			simulation_destroy(t_simulation *simulation);
int				simulation_run(t_simulation *simulation);
int				simulation_stopped(t_simulation *simulation);
void			simulation_stop(t_simulation *simulation);

#endif
