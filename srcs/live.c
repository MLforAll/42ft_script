/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/09 03:59:44 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/14 05:43:59 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "ft_script.h"

static void		script_write_record(t_typescript *ts, \
									const char *data, \
									size_t sz, \
									enum e_rts_direction dir)
{
	t_rts_record	rec;
	struct timeval	sp;

	if (gettimeofday(&sp, NULL) != 0)
		return ;
	ft_bzero(&rec, sizeof(t_rts_record));
	rec.size = (t_uint32)sz;
	rec.timestamp = (t_uint32)sp.tv_sec;
	rec.utimestamp = (t_uint32)sp.tv_usec;
	rec.direction = (t_uint32)dir;
	(void)write(ts->fd, &rec, sizeof(t_rts_record));
	if (data && sz)
		(void)write(ts->fd, data, sz);
}

static t_uint8	script_write(t_pty *p, \
							t_typescript *ts, \
							fd_set *setref, \
							t_opts *opts)
{
	char	tsb[1024];
	ssize_t	rb;

	if (FD_ISSET(STDIN_FILENO, setref))
	{
		if ((rb = read(STDIN_FILENO, tsb, sizeof(tsb))) < 1)
			return (FALSE);
		(void)write(p->fdm, tsb, (size_t)rb);
		if (opts->switches & kSwitchR)
			script_write_record(ts, tsb, (size_t)rb, kDirectionInput);
	}
	if (FD_ISSET(p->fdm, setref))
	{
		if ((rb = read(p->fdm, tsb, sizeof(tsb))) < 1)
			return (FALSE);
		(void)write(STDOUT_FILENO, tsb, (size_t)rb);
		if (opts->switches & kSwitchR)
			script_write_record(ts, tsb, (size_t)rb, kDirectionOutput);
		else
			(opts->switches & kSwitchF) ? (void)write(ts->fd, tsb, (size_t)rb) \
									: ft_bwrite(ts->fd, tsb, (size_t)rb, NO);
	}
	return (TRUE);
}

static void		script(t_pty *p, t_typescript *ts, t_cmd *cmd, t_opts *opts)
{
	fd_set	ss;
	t_uint8	recorded;

	if ((recorded = (opts->switches & kSwitchR) != 0))
		script_write_record(ts, NULL, 0, kDirectionStart);
	if (!(opts->switches & kSwitchQ))
		announce_script(ts, cmd, YES, recorded);
	while (TRUE)
	{
		FD_ZERO(&ss);
		FD_SET(p->fdm, &ss);
		FD_SET(STDIN_FILENO, &ss);
		if (select(p->fdm + 1, &ss, NULL, NULL, NULL) < 1 \
			|| !script_write(p, ts, &ss, opts))
			break ;
	}
	if (!(opts->switches & kSwitchF))
		ft_bwrite(ts->fd, NULL, 0, YES);
	if (opts->switches & kSwitchR)
		script_write_record(ts, NULL, 0, kDirectionStart);
	if (!(opts->switches & kSwitchQ))
		announce_script(ts, cmd, NO, recorded);
}

static void		exec_process(t_cmd *cmd)
{
	extern char		**environ;
	t_stkhp_buff	nav_buff;
	char			**nav;
	size_t			tmp;

	if (!cmd->runenv)
	{
		(void)execve(cmd->path, cmd->args, environ);
		return ;
	}
	tmp = (ft_tablen(cmd->args) + 1) * sizeof(char *);
	if (!(nav = (char **)stkhp_buff_alloc(&nav_buff, tmp)))
		return ;
	*nav = "/usr/bin/env";
	tmp = 0;
	while (cmd->args[tmp++])
		nav[tmp] = cmd->args[tmp - 1];
	nav[tmp] = NULL;
	(void)execve(*nav, nav, environ);
}

int				fork_process(t_pty *pty, \
								t_cmd *cmd, \
								t_typescript *ts, \
								t_opts *opts)
{
	pid_t	pid;
	int		status;

	if ((pid = fork()) == -1)
		sfatal("fork() failed", 1);
	if (pid == 0)
	{
		pty_child_attach(pty);
		exec_process(cmd);
		ft_putendl("./ft_script: Could not exec");
		_exit(15);
	}
	(void)close(pty->fds);
	script(pty, ts, cmd, opts);
	if (waitpid(pid, &status, 0) != pid)
		status = EXIT_FAILURE;
	(void)close(pty->fdm);
	return (WEXITSTATUS(status));
}
