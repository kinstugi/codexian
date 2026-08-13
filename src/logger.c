/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:41:34 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:41:38 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"
#include <stdio.h>

void	logger(t_simulation *simulation, int coder_id, const char *message)
{
	long	elapsed;

	elapsed = get_elapsed_time_ms(simulation);
	pthread_mutex_lock(&simulation->logging);
	printf("%ld %d %s\n", elapsed, coder_id, message);
	pthread_mutex_unlock(&simulation->logging);
}
