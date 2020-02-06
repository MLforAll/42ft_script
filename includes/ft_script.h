/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:09:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/06 04:14:01 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SCRIPT_H
# define FT_SCRIPT_H

# include <termios.h>
# include "libft.h"

# define USE_LIBC_DIR		1
# define PTSNAME_MAX_SIZE	128

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
	const char	**args;
	int			shell;
	int			reserved;
}				t_cmd;

/*
** cmdpath
*/

const char		*cmd_path(const char *cmd);

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
** Utils
*/

int				ft_ptsname_r(int fd, char *buff, size_t max);
char			*ft_ptsname(int fd);
int				ft_tcgetattr(int fd, struct termios *t);
int				ft_tcsetattr(int fd, int opts, const struct termios *t);

void			ft_sfatal(const char *msg, int status);
void			ft_putstr_fds(const char *s, int fd1, int fd2);
void			ft_putendl_fds(const char *s, int fd1, int fd2);
void			ft_mwrites(int fd1, int fd2, const void *buff, size_t nbytes);

#endif
