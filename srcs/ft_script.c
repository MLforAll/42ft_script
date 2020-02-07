/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelian <kelian@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:09:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/07 02:56:05 by kelian           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include "time.h"

#include "ft_script.h"

static uint8_t	send_stdin_to_fdm(t_pty *p)
{
	char	buff[16];
	ssize_t	rb;

	if ((rb = read(STDIN_FILENO, buff, sizeof(buff))) < 1)
		return (FALSE);
	(void)write(p->fdm, buff, rb);
	return (TRUE);
}

static void		announce_script(t_typescript *ts, uint8_t begin)
{
	struct timeval	tp;
	const char		*ct;
	int				rc;

	rc = gettimeofday(&tp, NULL);
	ct = (rc == 0) ? ctime(&tp.tv_sec) : "NULL";
	if (begin)
	{
		ft_putstr("Script started, output file is ");
		ft_putendl(ts->path);
		ft_putstr_fd("Script started on ", ts->fd);
		ft_putstr_fd(ct, ts->fd);
		return ;
	}
	ft_putstr("\nScript done, output file is ");
	ft_putendl(ts->path);
	ft_putstr_fd("\nScript done on ", ts->fd);
	ft_putstr_fd(ct, ts->fd);
}

static void		script(t_pty *p, t_typescript *ts, t_opts *opts)
{
	char	tsb[512];
	ssize_t	rb;
	fd_set	ss;

	(!(opts->switches & kSwitchQ)) ? announce_script(ts, YES) : 0;
	while (TRUE)
	{
		FD_ZERO(&ss);
		FD_SET(p->fdm, &ss);
		FD_SET(STDIN_FILENO, &ss);
		if (select(p->fdm + 1, &ss, NULL, NULL, NULL) < 1 \
			|| (FD_ISSET(STDIN_FILENO, &ss) && !send_stdin_to_fdm(p)))
			break ;
		if (FD_ISSET(p->fdm, &ss))
		{
			if ((rb = read(p->fdm, tsb, sizeof(tsb))) < 1)
				break ;
			(void)write(STDOUT_FILENO, tsb, rb);
			(opts->switches & kSwitchF) ? (void)write(ts->fd, tsb, rb) \
										: ft_bwrite(ts->fd, tsb, rb, NO);
		}
	}
	(!(opts->switches & kSwitchF)) ? ft_bwrite(ts->fd, NULL, 0, YES) : 0;
	(!(opts->switches & kSwitchQ)) ? announce_script(ts, NO) : 0;
}

static void		fork_process(t_pty *pty, \
								const char *cmdpath, \
								t_typescript *ts, \
								t_opts *opts)
{
	pid_t	pid;

	if ((pid = fork()) == -1)
		ft_sfatal("fork() failed", 1);
	if (pid == 0)
	{
		pty_child_attach(pty);
		(void)execl(cmdpath, cmdpath);
		_exit(127);
	}
	(void)close(pty->fds);
	script(pty, ts, opts);
	(void)waitpid(pid, NULL, 0);
	(void)close(pty->fdm);
}

int				main(int ac, const char **av)
{
	t_pty			pty;
	t_opts			opts;
	t_typescript	ts;

	if (!pty_new(&pty))
		ft_sfatal("Could not create new pseudo-terminal", 1);
	if (!pty_slave_open(&pty))
		ft_sfatal("Could not open slave portion of created pty", 1);
	if (!options_parse(ac, av, &opts))
		return (EXIT_FAILURE);
	ac -= opts.ind;
	av += opts.ind;
	ts.path = (ac > 1) ? av[1] : "typescript";
	if ((ts.fd = open(ts.path, O_WRONLY | O_CREAT \
		| ((opts.switches & kSwitchA) ? O_APPEND : O_TRUNC), 0644)) < 0)
		ft_sfatal("Could not open typescript file for writing", 1);
	if (!configure_inherited_tty(NO))
		ft_sfatal("Could not configure inherited tty", 1);
	fork_process(&pty, cmd_path((ac > 2) ? av[2] : NULL), &ts, &opts);
	(void)configure_inherited_tty(YES);
	(void)close(ts.fd);
	return (EXIT_SUCCESS);
}
