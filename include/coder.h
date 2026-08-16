/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:47:32 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:47:37 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

# include "types.h"

int				coder_acquire_dongles(t_coder *coder);
void			coder_release_dongles(t_coder *coder);
void			coder_set_state(t_coder *coder, t_coder_state state);
int				coder_should_run(t_coder *coder);
int				coder_wait_ms(t_simulation *simulation, long ms);
int				coder_begin_compile(t_coder *coder);
void			coder_complete_compile(t_coder *coder);
void			coder_finish(t_coder *coder);
int				wait_until_granted(t_coder *coder);
void	*coder_routine(void *arg);

#endif
