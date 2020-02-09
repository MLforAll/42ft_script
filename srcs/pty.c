/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pty.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:30:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/09 05:08:59 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ft_script.h"

t_uint8		pty_new(t_pty *out)
{
	if (!out || (out->fdm = open("/dev/ptmx", O_RDWR)) < 0 \
		|| ft_ptsname_r(out->fdm, out->ptsname, sizeof(out->ptsname)) != 0)
		return (FALSE);
	out->fds = -1;
	return (TRUE);
}

t_uint8		pty_slave_open(t_pty *p)
{
	struct winsize	ws;

	if (ioctl(p->fdm, TIOCPTYGRANT) != 0 || ioctl(p->fdm, TIOCPTYUNLK) != 0 \
		|| (p->fds = open(p->ptsname, O_RDWR)) == -1)
		return (FALSE);
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
		(void)ioctl(p->fds, TIOCSSIZE, &ws);
	return (TRUE);
}

inline void	pty_child_attach(t_pty *p)
{
	(void)close(p->fdm);
	(void)dup2(p->fds, STDIN_FILENO);
	(void)dup2(p->fds, STDOUT_FILENO);
	(void)dup2(p->fds, STDERR_FILENO);
	(void)close(p->fds);
	(void)setsid();
	(void)ioctl(STDIN_FILENO, TIOCSCTTY, YES);
}

inline void	pty_close(t_pty *p)
{
	(void)close(p->fdm);
	(void)close(p->fds);
}
