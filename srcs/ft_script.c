/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:09:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/06 05:47:29 by kdumarai         ###   ########.fr       */
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

static void		announce_script(const char *tsfile, int tsfd, uint8_t begin)
{
	struct timeval	tp;
	const char		*ct;
	int				rc;

	rc = gettimeofday(&tp, NULL);
	ct = (rc == 0) ? ctime(&tp.tv_sec) : "NULL";
	if (begin)
	{
		ft_putstr("Script started, output file is ");
		ft_putendl(tsfile);
		ft_putstr_fd("Script started on ", tsfd);
		ft_putstr_fd(ct, tsfd);
		return ;
	}
	ft_putstr("\nScript done, output file is ");
	ft_putendl(tsfile);
	ft_putstr_fd("\nScript done on ", tsfd);
	ft_putstr_fd(ct, tsfd);
}

static void		script(t_pty *p, const char *tsfile, t_opts *opts)
{
	int		tsfd;
	char	tsb[512];
	ssize_t	rb;
	fd_set	ss;

	if ((tsfd = open(tsfile, O_WRONLY | O_CREAT \
		| ((opts->switches & kSwitchA) ? O_APPEND : O_TRUNC), 0644)) < 0)
		return ;
	if (!(opts->switches & kSwitchQ))
		announce_script(tsfile, tsfd, YES);
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
			ft_mwrites(STDOUT_FILENO, tsfd, tsb, rb);
		}
	}
	if (!(opts->switches & kSwitchQ))
		announce_script(tsfile, tsfd, NO);
	(void)close(tsfd);
}

static void		fork_process(t_pty *pty, const char *cmdpath, const char *file, t_opts *opts)
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
	script(pty, file, opts);
	(void)waitpid(pid, NULL, 0);
	(void)close(pty->fdm);
}

int				main(int ac, const char **av)
{
	t_pty	pty;
	t_opts	opts;

	if (!pty_new(&pty))
		ft_sfatal("Could not create new pseudo-terminal", 1);
	if (!pty_slave_open(&pty))
		ft_sfatal("Could not open slave portion of created pty", 1);
	if (!options_parse(ac, av, &opts))
		return (EXIT_FAILURE);
	ac -= opts.ind;
	av += opts.ind;
	(void)configure_inherited_tty(NO);
	fork_process(&pty, \
		cmd_path((ac > 2) ? av[2] : NULL), \
		(ac > 1) ? av[1] : "typescript", \
		&opts);
	(void)configure_inherited_tty(YES);
	return (EXIT_SUCCESS);
}
