/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:09:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/14 03:34:01 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SCRIPT_H
# define FT_SCRIPT_H

# include <termios.h>
# include <time.h>
# include "libft.h"

# define USE_LIBC_DIR		0
# define PTSNAME_MAX_SIZE	128

/*
** Args Parsing
*/

# define OPTSTRING			"adFpqr"

enum			e_switches
{
	kSwitchA = 1 << 0,
	kSwitchD = 1 << 1,
	kSwitchF = 1 << 2,
	kSwitchP = 1 << 3,
	kSwitchQ = 1 << 4,
	kSwitchR = 1 << 5,
	kSwitchT = 1 << 6
};

typedef struct	s_opts
{
	const char	*arg;
	int			switches;
	int			ind;
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

/*
** Main
*/

int				fork_process(t_pty *pty, \
								t_cmd *cmd, \
								t_typescript *ts, \
								t_opts *opts);

t_uint8			next_record(t_rts_record **out, \
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

void			install_timer(t_typescript *ts, t_opts *opts);

/*
** Utils
*/

int				ft_ptsname_r(int fd, char *buff, size_t max);
char			*ft_ptsname(int fd);
int				ft_tcgetattr(int fd, struct termios *t);
int				ft_tcsetattr(int fd, int opts, const struct termios *t);

void			sfatal(const char *msg, int status) __attribute__((noreturn));
void			ft_putstr_fds(const char *s, int fd1, int fd2);
void			ft_putendl_fds(const char *s, int fd1, int fd2);
void			ft_mwrites(int fd1, int fd2, const void *buff, size_t nbytes);
void			ft_bwrite(int fd, const void *data, size_t nbytes, t_uint8 rst);

#endif
