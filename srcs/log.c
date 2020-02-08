/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/08 01:03:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/08 01:29:53 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/time.h>
#include "ft_script.h"

inline static void	announce_script_command(t_cmd *cmd, int tsfd)
{
	char	**bw;

	ft_putstr_fd("command:", tsfd);
	bw = cmd->args;
	while (*bw)
	{
		ft_putchar_fd(' ', tsfd);
		ft_putstr_fd(*bw++, tsfd);
	}
	ft_putchar_fd('\n', tsfd);
}

void				announce_script(t_typescript *ts, t_cmd *cmd, t_uint8 begin)
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
		if (!cmd->shell)
			announce_script_command(cmd, ts->fd);
		return ;
	}
	ft_putstr("\nScript done, output file is ");
	ft_putendl(ts->path);
	ft_putstr_fd("\nScript done on ", ts->fd);
	ft_putstr_fd(ct, ts->fd);
}
