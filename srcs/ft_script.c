/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:09:08 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/09 04:38:42 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "ft_script.h"

static int		do_work(int ac, const char **av, t_pty *pty, t_opts *opts)
{
	t_typescript	ts;
	t_cmd			cmd;
	int				exval;
	int				mode;

	ts.path = (ac > 1) ? av[1] : "typescript";
	if (opts->switches & kSwitchP)
		mode = O_RDONLY;
	else
		mode = O_WRONLY | O_CREAT \
			| ((opts->switches & kSwitchA) ? O_APPEND : O_TRUNC);
	if ((ts.fd = open(ts.path, mode, 0644)) < 0)
		ft_sfatal("Could not open typescript file", 1);
	if (opts->switches & kSwitchP)
		exval = play_file(&ts, opts);
	else
	{
		if (!configure_inherited_tty(NO))
			ft_sfatal("Could not configure inherited tty", 1);
		cmd_init(&cmd, (ac > 2) ? av + 2 : NULL);
		exval = fork_process(pty, &cmd, &ts, opts);
		(void)configure_inherited_tty(YES);
	}
	(void)close(ts.fd);
	return (exval);
}

int				main(int ac, const char **av)
{
	t_pty			pty;
	t_opts			opts;

	if (!pty_new(&pty))
		ft_sfatal("Could not create new pseudo-terminal", 1);
	if (!pty_slave_open(&pty))
		ft_sfatal("Could not open slave portion of created pty", 1);
	if (!options_parse(ac, av, &opts))
		return (EXIT_FAILURE);
	ac -= opts.ind;
	av += opts.ind;
	return (do_work(ac, av, &pty, &opts));
}
