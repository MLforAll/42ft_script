/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/09 03:59:44 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/22 17:57:26 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>

#include "ft_script.h"

static void				script_write_record(t_typescript *ts, \
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

inline static t_uint8	script_kmode_chk_echo(t_pty *p)
{
	struct termios	slvt;

	if (ft_tcgetattr(p->fdm, &slvt) != 0)
		return (NO);
	return ((slvt.c_lflag & ECHO) != 0);
}

inline static t_uint8	script_write(t_pty *p, \
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
		if ((opts->switches & kSwitchK) && !script_kmode_chk_echo(p))
			(void)ft_bwrite(ts->fd, tsb, (size_t)rb, (t_uint8)opts->tsw_rst);
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
			(void)ft_bwrite(ts->fd, tsb, (size_t)rb, (t_uint8)opts->tsw_rst);
	}
	return (TRUE);
}

inline static void		script_loop(t_pty *p, \
									t_typescript *ts, \
									t_opts *opts)
{
	fd_set	ss;

	while (TRUE)
	{
		FD_ZERO(&ss);
		FD_SET(p->fdm, &ss);
		FD_SET(STDIN_FILENO, &ss);
		if (select(p->fdm + 1, &ss, NULL, NULL, NULL) < 1)
		{
			if (g_alarm_toggled)
			{
				ft_bwrite(ts->fd, NULL, 0, YES);
				g_alarm_toggled = NO;
				continue ;
			}
			break ;
		}
		if (!script_write(p, ts, &ss, opts))
			break ;
	}
}

void					script(t_pty *p, \
								t_typescript *ts, \
								t_cmd *cmd, \
								t_opts *opts)
{
	t_uint8	recorded;

	if ((recorded = (opts->switches & kSwitchR) != 0))
		script_write_record(ts, NULL, 0, kDirectionStart);
	if (!(opts->switches & kSwitchQ))
		announce_script(ts, cmd, YES, recorded);
	script_loop(p, ts, opts);
	if (!(opts->switches & kSwitchF))
		(void)ft_bwrite(ts->fd, NULL, 0, YES);
	if (opts->switches & kSwitchR)
		script_write_record(ts, NULL, 0, kDirectionStart);
	if (!(opts->switches & kSwitchQ))
		announce_script(ts, cmd, NO, recorded);
}
