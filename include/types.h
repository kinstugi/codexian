/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkusi-fr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 02:50:23 by bkusi-fr          #+#    #+#             */
/*   Updated: 2026/08/13 02:50:28 by bkusi-fr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# include <pthread.h>

typedef struct s_config		t_config;
typedef struct s_simulation	t_simulation;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;
typedef struct s_heap_item	t_heap_item;

typedef enum e_sched_type
{
	EDF,
	FIFO
}							t_sched_type;

typedef enum e_coder_state
{
	WAITING,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	DONE,
	BURNED_OUT
}							t_coder_state;

typedef struct s_config
{
	int						number_of_coders;
	int						time_to_burnout;
	int						time_to_compile;
	int						time_to_debug;
	int						time_to_refactor;
	int						number_of_compiles_required;
	int						dongle_cooldown;
	t_sched_type			scheduler;
}							t_config;

typedef struct s_coder
{
	int						id;
	pthread_t				thread;
	t_dongle				*left_dongle;
	t_dongle				*right_dongle;
	int						compile_count;
	long					last_compile_start;
	t_coder_state			state;
	t_simulation			*simulation;
}							t_coder;

typedef struct s_dongle
{
	int						id;
	pthread_mutex_t			mutex;
	long					available_at;
	int						owner;
}							t_dongle;

typedef struct s_sim_sync
{
	pthread_mutex_t			mutex;
	pthread_cond_t			condition;
}							t_sim_sync;

typedef struct s_simulation
{
	t_config				*configuration;
	t_coder					*coders;
	t_dongle				*dongles;
	long					start_time;
	int						stop_flag;
	int						stop_reason;
	pthread_t				monitor;
	pthread_mutex_t			logging;
	t_sim_sync				sync;
}							t_simulation;

typedef struct s_heap_item
{
	long					key;
	long					tie;
	void					*data;
}							t_heap_item;

typedef struct s_heap
{
	t_heap_item				*items;
	int						capacity;
	int						size;
}							t_heap;

#endif
