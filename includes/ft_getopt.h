/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_getopt.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/06 02:15:52 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/06 03:04:03 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GETOPT_H
# define FT_GETOPT_H

extern const char	*g_optarg;
extern int			g_optind;
extern int			g_optopt;
extern int			g_opterr;
extern int			g_optreset;

int		ft_getopt(int ac, const char **av, const char *optstring);

#endif
