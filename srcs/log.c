/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/08 01:03:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/14 02:07:12 by kdumarai         ###   ########.fr       */
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

void				announce_script_time(int fd, \
										time_t time, \
										t_uint8 ftime, \
										t_uint8 begin)
{
	struct timeval	tp;
	const char		*ct;
	int				rc;

	if (ftime)
	{
		rc = gettimeofday(&tp, NULL);
		time = (time_t)tp.tv_sec;
	}
	else
		rc = 0;
	ct = (rc == 0) ? ctime(&time) : NULL;
	ft_putstr_fd((begin) ? "Script started on " : "\nScript done on ", fd);
	ft_putstrsec_fd(ct, fd);
}

void				announce_script(t_typescript *ts, \
									t_cmd *cmd, \
									t_uint8 begin, \
									t_uint8 recorded)
{
	if (begin)
	{
		ft_putstr("Script started, output file is ");
		ft_putendl(ts->path);
		if (!recorded)
		{
			announce_script_time(ts->fd, 0, YES, YES);
			if (!cmd->shell)
				announce_script_command(cmd, ts->fd);
		}
		return ;
	}
	ft_putstr("\nScript done, output file is ");
	ft_putendl(ts->path);
	if (!recorded)
		announce_script_time(ts->fd, 0, YES, NO);
}
