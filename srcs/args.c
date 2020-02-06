/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/06 05:19:16 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/06 05:47:42 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"
#include "ft_getopt.h"

t_uint8	options_parse(int ac, const char **av, t_opts *ptr)
{
	int			rc;
	const char	*tmp;

	ptr->switches = 0;
	while ((rc = ft_getopt(ac, av, OPTSTRING)) != -1)
	{
		if ((tmp = ft_strchr(OPTSTRING, rc)))
			ptr->switches |= (enum e_switches)(1 << (tmp - OPTSTRING));
		if (rc == 'F')
			ptr->f_arg = g_optarg;
	}
	ptr->ind = g_optind;
	return ((rc == -1) ? YES : NO);
}
