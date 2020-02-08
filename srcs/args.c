/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelian <kelian@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/06 05:19:16 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/07 02:15:44 by kelian           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"
#include "ft_getopt.h"

static t_uint8	options_parse_getopt(int ac, const char **av, t_opts *ptr)
{
	int			rc;
	const char	*tmp;

	ptr->switches = 0;
	while ((rc = ft_getopt(ac, av, OPTSTRING)) != -1 && rc != '?')
	{
		if (rc == 'F')
		{
			ptr->switches |= kSwitchF;
			ptr->f_arg = g_optarg;
		}
		else if ((tmp = ft_strchr(OPTSTRING, rc)))
			ptr->switches |= (enum e_switches)(1 << (tmp - OPTSTRING));
	}
	ptr->ind = g_optind - 1;
	return ((rc == -1) ? YES : NO);
}

t_uint8			options_parse(int ac, const char **av, t_opts *ptr)
{
	int		rc;
	int		idx;

	ptr->f_arg = NULL;
	if (ft_strchr(OPTSTRING, ':'))
		return (options_parse_getopt(ac, av, ptr));
	idx = 1;
	if ((rc = ft_args_opts(av, &idx, OPTSTRING, NULL)) < 0)
	{
		ft_getopt_error(av, -rc, GETOPT_ERR_ILL);
		return (NO);
	}
	ptr->switches = (enum e_switches)rc;
	ptr->ind = idx - 1;
	return (YES);
}