/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_internal.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:49:54 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:49:58 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMULATION_INTERNAL_H
# define SIMULATION_INTERNAL_H

# include "types.h"

int		init_dongles(t_simulation *simulation);
int		init_coders(t_simulation *simulation);
void	join_coders(t_simulation *simulation, int count);

#endif
