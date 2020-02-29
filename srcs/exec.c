/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/22 17:02:52 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/29 21:59:50 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "ft_script.h"

/*
** FORGIVEME: malloc() (or ft_malloc()) would have been nice
*/

static char		*sp_newenv(const char *name, const char *value)
{
	char	*map;
	size_t	sz;
	size_t	nsz;

	nsz = ft_strlen(name);
	sz = (nsz + ft_strlen(value) + 2) * sizeof(char);
	map = (char *)mmap(NULL, sz, PROT_READ | PROT_WRITE,
		MAP_ANON | MAP_PRIVATE, -1, 0);
	if (map)
	{
		(void)ft_strcpy(map, name);
		map[nsz] = '=';
		(void)ft_strcpy(map + nsz + 1, value);
	}
	return (map);
}

static t_uint8	sp_addenv(const char *name, const char *value)
{
	extern char			**environ;
	size_t				len;
	static t_stkhp_buff	rp;
	t_stkhp_buff		np;
	t_stkhp_buff		*pp;

	pp = (rp.ptr) ? &np : &rp;
	len = ft_tablen(environ);
	if (!stkhp_buff_alloc(pp, (len + 2) * sizeof(char *)))
		return (-1);
	(void)ft_memcpy(pp->ptr, environ, len * sizeof(char *));
	((char **)pp->ptr)[len++] = sp_newenv(name, value);
	((char **)pp->ptr)[len] = NULL;
	if (pp == &np)
	{
		stkhp_buff_free(&rp);
		(void)ft_memcpy(&rp, &np, sizeof(t_stkhp_buff));
	}
	environ = (char **)rp.ptr;
	return (0);
}

static int		waitandkill(pid_t pid)
{
	int		status;
	pid_t	rc;

	if ((rc = waitpid(pid, &status, WNOHANG)) == pid)
		return (WEXITSTATUS(status));
	else if (rc < 0)
		return (EXIT_FAILURE);
	(void)kill(pid, SIGKILL);
	if (waitpid(pid, &status, WNOHANG) == pid)
		return (WEXITSTATUS(status));
	return (EXIT_FAILURE);
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
	int		rc;

	if ((pid = fork()) == -1)
		sfatal("fork() failed", 1);
	if (pid == 0)
	{
		pty_child_attach(pty);
		(void)sp_addenv("SCRIPT", ts->path);
		exec_process(cmd);
		ft_putendl("./ft_script: Could not exec");
		_exit(15);
	}
	(void)close(pty->fds);
	install_timer(opts->flush_itv);
	script(pty, ts, cmd, opts);
	rc = waitandkill(pid);
	(void)close(pty->fdm);
	return (rc);
}
