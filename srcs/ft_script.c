/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:09:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/05 22:03:37 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>

#include "ft_script.h"

static void	script(t_pty *p, const char *tsfile)
{
	int		tsfd;
	char	tsb[512];
	ssize_t	rb;
	fd_set	ss;

	if ((tsfd = open(tsfile, O_WRONLY | O_CREAT, 0644)) < 0)
		return ;
	while (TRUE)
	{
		FD_ZERO(&ss);
		FD_SET(p->fdm, &ss);
		FD_SET(STDIN_FILENO, &ss);
		if (select(p->fdm + 1, &ss, NULL, NULL, NULL) < 1)
			break ;
		if (FD_ISSET(STDIN_FILENO, &ss))
		{
			if ((rb = read(STDIN_FILENO, tsb, sizeof(tsb))) < 1)
				break ;
			(void)write(p->fdm, tsb, rb);
		}
		else if (FD_ISSET(p->fdm, &ss))
		{
			if ((rb = read(p->fdm, tsb, sizeof(tsb))) < 1)
				break ;
			(void)write(STDOUT_FILENO, tsb, rb);
			(void)write(tsfd, tsb, rb);
		}
	}
	(void)close(tsfd);
}

const char	*shell_path(void)
{
	extern char	**environ;
	const char	*tmp;
	char		**bw;

	bw = environ;
	while (bw && *bw)
	{
		if (ft_strstart(*bw, "SHELL") && (tmp = ft_strchr(*bw, '=')))
			return (tmp + 1);
		bw++;
	}
	return ("/bin/sh");
}

int			main(int ac, const char **av)
{
	const char	*cmdpath;
	const char	*file;
	t_pty		pty;
	pid_t		pid;

	if (!pty_new(&pty))
		ft_sfatal("Could not create new pseudo-terminal", 1);
	if (!pty_slave_open(&pty))
		ft_sfatal("Could not open slave portion of created pty", 1);
	cmdpath = (ac > 2) ? av[2] : shell_path();
	file = (ac > 1) ? av[1] : "typescript";
	(void)configure_inherited_tty(NO);
	if ((pid = fork()) == -1)
		ft_sfatal("fork() failed", 1);
	if (pid == 0)
	{
		pty_child_attach(&pty);
		(void)execl(cmdpath, cmdpath, NULL);
		_exit(127);
	}
	(void)close(pty.fds);
	ft_putstr("Script started, output file is ");
	ft_putendl(file);
	script(&pty, file);
	(void)waitpid(pid, NULL, 0);
	(void)close(pty.fdm);
	ft_putstr("\nScript done, output file is ");
	ft_putendl(file);
	(void)configure_inherited_tty(YES);
	return (EXIT_SUCCESS);
}
