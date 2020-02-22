/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/22 17:02:52 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/22 17:46:18 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>

#include "ft_script.h"

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
	install_timer(opts->flush_itv);
	script(pty, ts, cmd, opts);
	if (waitpid(pid, &status, 0) != pid)
		status = EXIT_FAILURE;
	(void)close(pty->fdm);
	return (WEXITSTATUS(status));
}
