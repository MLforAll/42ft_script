/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:09:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/08 21:31:43 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/mman.h>

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

static void		script(t_pty *p, t_typescript *ts, t_cmd *cmd, t_opts *opts)
{
	char	tsb[512];
	ssize_t	rb;
	fd_set	ss;

	(!(opts->switches & kSwitchQ)) ? announce_script(ts, cmd, YES) : 0;
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
	(!(opts->switches & kSwitchQ)) ? announce_script(ts, cmd, NO) : 0;
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

static int		fork_process(t_pty *pty, \
								t_cmd *cmd, \
								t_typescript *ts, \
								t_opts *opts)
{
	pid_t		pid;
	int			status;

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

int				main(int ac, const char **av)
{
	t_pty			pty;
	t_opts			opts;
	t_typescript	ts;
	t_cmd			cmd;
	int				exval;

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
	cmd_init(&cmd, (ac > 2) ? av + 2 : NULL);
	exval = fork_process(&pty, &cmd, &ts, &opts);
	(void)configure_inherited_tty(YES);
	(void)close(ts.fd);
	return (exval);
}
