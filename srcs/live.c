/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/09 03:59:44 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/12 06:47:16 by kdumarai         ###   ########.fr       */
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
		(void)write(p->fdm, tsb, rb);
		if (opts->switches & kSwitchR)
			script_write_record(ts, tsb, rb, kDirectionInput);
	}
	if (FD_ISSET(p->fdm, setref))
	{
		if ((rb = read(p->fdm, tsb, sizeof(tsb))) < 1)
			return (FALSE);
		(void)write(STDOUT_FILENO, tsb, rb);
		if (opts->switches & kSwitchR)
			script_write_record(ts, tsb, rb, kDirectionOutput);
		else
			(opts->switches & kSwitchF) ? (void)write(ts->fd, tsb, rb) \
										: ft_bwrite(ts->fd, tsb, rb, NO);
	}
	return (TRUE);
}

static void		script(t_pty *p, t_typescript *ts, t_cmd *cmd, t_opts *opts)
{
	fd_set	ss;

	if (opts->switches & kSwitchR)
		script_write_record(ts, NULL, 0, kDirectionStart);
	else if (!(opts->switches & kSwitchQ))
		announce_script(ts, cmd, YES);
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
	else if (!(opts->switches & kSwitchQ))
		announce_script(ts, cmd, NO);
}

static void		exec_process(t_cmd *cmd)
{
	extern char	**environ;
	void		*map;
	size_t		sz;
	size_t		idx;

	if (!cmd->runenv)
	{
		(void)execve(cmd->path, cmd->args, environ);
		return ;
	}
	sz = (ft_tablen(cmd->args) + 1) * sizeof(char *);
	map = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (!map)
		return ;
	*(char **)map = "/usr/bin/env";
	idx = 0;
	while (cmd->args[idx++])
		((char **)map)[idx] = cmd->args[idx - 1];
	((char **)map)[idx] = NULL;
	(void)execve(*(char **)map, map, environ);
}

int				fork_process(t_pty *pty, \
								t_cmd *cmd, \
								t_typescript *ts, \
								t_opts *opts)
{
	pid_t	pid;
	int		status;

	if ((pid = fork()) == -1)
		ft_sfatal("fork() failed", 1);
	if (pid == 0)
	{
		pty_child_attach(pty);
		exec_process(cmd);
		ft_putstr("./ft_script: ");
		if (cmd->path)
		{
			ft_putstr(cmd->path);
			ft_putstr(": ");
		}
		ft_putendl("Could not exec");
		_exit(15);
	}
	(void)close(pty->fds);
	script(pty, ts, cmd, opts);
	if (waitpid(pid, &status, 0) != pid)
		status = EXIT_FAILURE;
	(void)close(pty->fdm);
	return (WEXITSTATUS(status));
}
