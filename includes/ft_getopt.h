/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_getopt.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelian <kelian@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/06 02:15:52 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/07 01:26:00 by kelian           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GETOPT_H
# define FT_GETOPT_H

# define GETOPT_ERR_NOMORE	-1
# define GETOPT_ERR_ILL		-2
# define GETOPT_ERR_REQ		-3

extern const char	*g_optarg;
extern int			g_optind;
extern int			g_optopt;
extern int			g_opterr;
extern int			g_optreset;

int		ft_getopt(int ac, const char **av, const char *optstring);
void	ft_getopt_error(const char **av, int opt, int type);

#endif
