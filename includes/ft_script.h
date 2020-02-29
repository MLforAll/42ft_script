/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:09:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/29 18:12:52 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SCRIPT_H
# define FT_SCRIPT_H

# include <termios.h>
# include <time.h>
# include <signal.h>

# include "libft.h"

# define USE_LIBC_DIR		0
# define PTSNAME_MAX_SIZE	128

/*
** Globals
*/

extern volatile sig_atomic_t	g_alarm_toggled;

/*
** Args Parsing
*/

# define OPTSTRING			"adFkpqrt:"

enum			e_switches
{
	kSwitchA = 1 << 0,
	kSwitchD = 1 << 1,
	kSwitchF = 1 << 2,
	kSwitchK = 1 << 3,
	kSwitchP = 1 << 4,
	kSwitchQ = 1 << 5,
	kSwitchR = 1 << 6,
	kSwitchT = 1 << 7
};

typedef struct	s_opts
{
	enum e_switches	switches;
	int				flush_itv;
	int				tsw_rst;
	int				ind;
}				t_opts;

/*
** Types
*/

typedef struct	s_pty
{
	int		fdm;
	int		fds;
	char	ptsname[PTSNAME_MAX_SIZE];
}				t_pty;

typedef struct	s_cmd
{
	const char	*path;
	char		**args;
	char		*shargs[3];
	int			shell;
	int			runenv;
}				t_cmd;

typedef struct	s_typescript
{
	const char	*path;
	int			fd;
	int			reserved;
}				t_typescript;

enum			e_rts_direction
{
	kDirectionAll = -1,
	kDirectionStart = 's',
	kDirectionOutput = 'o',
	kDirectionInput = 'i',
	kDirectionEnd = 'e'
};

typedef struct	s_rts_record
{
	t_uint64	size;
	t_uint64	timestamp;
	t_uint32	utimestamp;
	t_uint32	direction;
}				t_rts_record;

typedef struct	s_rts
{
	void			*map;
	size_t			sz;
	void			*endptr;
	t_rts_record	*firstr;
}				t_rts;

typedef struct	s_stkhp_buff
{
	unsigned char	r_stk[512];
	void			*r_map_ptr;
	size_t			r_map_sz;
	void			*ptr;
}				t_stkhp_buff;

/*
** Main
*/

void			script(t_pty *p, t_typescript *ts, t_cmd *cmd, t_opts *opts);

int				fork_process(t_pty *pty, \
								t_cmd *cmd, \
								t_typescript *ts, \
								t_opts *opts);

int				next_record(t_rts_record **out, \
						t_rts *rts, \
						t_rts_record *r, \
						enum e_rts_direction not_dir);
int				play_file(t_typescript *ts, t_opts *opts);

/*
** Logs
*/

void			announce_script_time(int fd, \
									time_t time, \
									t_uint8 ftime, \
									t_uint8 begin);
void			announce_script(t_typescript *ts, \
								t_cmd *cmd, \
								t_uint8 begin, \
								t_uint8 recorded);

/*
** Options
*/

t_uint8			options_parse(int ac, const char **av, t_opts *ptr);

/*
** cmd
*/

void			cmd_init(t_cmd *ptr, const char **av);

/*
** Term
*/

t_uint8			configure_inherited_tty(t_uint8 reset);
int				ft_tcsetattr(int fd, int opts, const struct termios *t);

/*
** Pty
*/

t_uint8			pty_new(t_pty *out);
t_uint8			pty_slave_open(t_pty *p);
void			pty_child_attach(t_pty *p);
void			pty_close(t_pty *p);

/*
** Bonuses
*/

void			install_timer(int itv);
void			ft_signal(int sig, void (*handler)(int));

/*
** Utils
*/

int				ft_ptsname_r(int fd, char *buff, size_t max);
char			*ft_ptsname(int fd);

void			sfatal(const char *msg, int status) __attribute__((noreturn));
ssize_t			ft_bwrite(int fd, const void *data, size_t nbytes, t_uint8 rst);

void			*stkhp_buff_alloc(t_stkhp_buff *out, size_t sz);
void			stkhp_buff_free(t_stkhp_buff *buff);

#endif
