/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:47:56 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:48:08 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DONGLE_H
# define DONGLE_H

/*
** Dongle resource model, mutex ownership, and cooldown.
** Implemented in milestone M4.
*/

# include "types.h"

int		dongle_init(t_dongle *dongle, int id);
void	dongle_destroy(t_dongle *dongle);
int		dongle_is_free(t_dongle *dongle);
void	dongle_claim(t_dongle *dongle, int coder_id);
void	dongles_release_both(t_dongle *a, t_dongle *b, long cooldown);
long	dongle_wake_time(t_dongle *dongle, long now);

#endif
