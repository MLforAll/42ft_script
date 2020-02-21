/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/14 03:20:16 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/21 08:18:46 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <sys/time.h>

#include "ft_script.h"

volatile sig_atomic_t	g_alarm_toggled;

static void				alarm_sighandler(int sig)
{
	if (sig == SIGALRM)
		g_alarm_toggled = YES;
}

void					install_timer(t_opts *opts)
{
	struct sigaction	sa;
	struct itimerval	timer;

	sa.sa_handler = &alarm_sighandler;
	sa.sa_mask = 0;
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGALRM, &sa, NULL) != 0)
		return ;
	timer.it_interval.tv_sec = opts->arg;
	timer.it_interval.tv_usec = 0;
	timer.it_value = timer.it_interval;
	(void)setitimer(ITIMER_REAL, &timer, NULL);
}
