/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:49:37 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:49:43 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
# define SCHEDULER_H

# include "types.h"

int		scheduler_init(t_scheduler *sched, int n, t_sched_type type);
void	scheduler_destroy(t_scheduler *sched);
void	scheduler_submit(t_simulation *sim, t_coder *coder);
void	scheduler_grant(t_simulation *sim);

#endif
