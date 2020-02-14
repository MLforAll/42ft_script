/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/14 03:20:16 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/14 05:26:11 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <sys/time.h>

#include "ft_script.h"

#if 0

volatile sig_atomic_t	g_alarm_sigfd;

static void				alarm_sighandler(int sig)
{
	(void)sig;
	if (g_alarm_sigfd != 0)
		ft_bwrite((int)g_alarm_sigfd, NULL, 0, YES);
}

void					install_timer(t_typescript *ts, t_opts *opts)
{
	struct sigaction	sa;
	struct itimerval	timer;

	sa.sa_handler = &alarm_sighandler;
	sa.sa_mask = 0;
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGALRM, &sa, NULL) != 0)
		return ;
	g_alarm_sigfd = ts->fd;
	timer.it_interval.tv_sec = ft_atoi(opts->arg);
	timer.it_interval.tv_usec = 0;
	timer.it_value = timer.it_interval;
	(void)setitimer(ITIMER_REAL, &timer, NULL);
}

#endif
