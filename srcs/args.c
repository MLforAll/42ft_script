/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/06 05:19:16 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/22 18:12:08 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"

static t_uint8	options_parse_getopt(int ac, const char **av, t_opts *ptr)
{
	int			rc;
	const char	*tmp;

	ptr->switches = (enum e_switches)0;
	while ((rc = ft_getopt(ac, av, OPTSTRING)) != -1 && rc != '?')
	{
		if (rc == 't')
		{
			ptr->switches |= kSwitchT;
			ptr->flush_itv = ft_atoi(g_optarg);
		}
		else if ((tmp = ft_strchr(OPTSTRING, rc)))
			ptr->switches |= (enum e_switches)(1 << (tmp - OPTSTRING));
	}
	ptr->tsw_rst = (ptr->switches & kSwitchF) != 0;
	ptr->ind = g_optind - 1;
	return ((rc == -1) ? YES : NO);
}

t_uint8			options_parse(int ac, const char **av, t_opts *ptr)
{
	int		rc;
	int		idx;

	ptr->flush_itv = 30;
	if (ft_strchr(OPTSTRING, ':'))
		return (options_parse_getopt(ac, av, ptr));
	idx = 1;
	if ((rc = ft_args_opts(av, &idx, OPTSTRING, NULL)) < 0)
	{
		ft_getopt_error(av, -rc, GETOPT_ERR_ILL);
		return (NO);
	}
	ptr->switches = (enum e_switches)rc;
	ptr->tsw_rst = (ptr->switches & kSwitchF) != 0;
	ptr->ind = idx - 1;
	return (YES);
}
