/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelian <kelian@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/06 05:19:16 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/06 23:25:50 by kelian           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"
#include "ft_getopt.h"

t_uint8	options_parse(int ac, const char **av, t_opts *ptr)
{
	int			rc;
	const char	*tmp;

	ptr->switches = 0;
	ptr->f_arg = NULL;
	while ((rc = ft_getopt(ac, av, OPTSTRING)) != -1)
	{
		if ((tmp = ft_strchr(OPTSTRING, rc)))
			ptr->switches |= (enum e_switches)(1 << (tmp - OPTSTRING));
		if (rc == 'F')
			ptr->f_arg = g_optarg;
	}
	ptr->ind = g_optind - 1;
	return ((rc == -1) ? YES : NO);
}
