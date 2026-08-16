/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_internal.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:43:53 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:43:57 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_INTERNAL_H
# define SCHEDULER_INTERNAL_H

# include "types.h"

long	scheduler_request_key(t_scheduler *sched, t_coder *coder);
long	scheduler_request_tie(t_coder *coder);

#endif
