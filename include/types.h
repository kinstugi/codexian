#ifndef TYPES_H
# define TYPES_H

# include <pthread.h>

typedef struct s_config		t_config;
typedef struct s_simulation	t_simulation;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;

typedef enum e_sched_type
{
	EDF,
	FIFO
}							sched_type;

typedef enum e_coder_state
{
	WAITING,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	DONE,
	BURNED_OUT
}							coder_state;

typedef struct s_config
{
	int						number_of_coders;
	int						time_to_burnout;
	int						time_to_compile;
	int						time_to_debug;
	int						time_to_refactor;
	int						number_of_compiles_required;
	int						dongle_cooldown;
	sched_type				scheduler;
}							t_config;

typedef struct s_coder
{
	int						id;
	pthread_t				thread;
	t_dongle				*left_dongle;
	t_dongle				*right_dongle;
	int						compile_count;
	int						last_compile_start;
	coder_state				state;
	t_simulation			*simulation;
}							t_coder;

typedef struct s_dongle
{
	int						id;
	pthread_mutex_t			mutex;
	int						available_at;
	int						scheduler;
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

#endif
