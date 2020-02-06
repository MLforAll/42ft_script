/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:11:02 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/06 00:54:15 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/uio.h>

#include "ft_script.h"

void		ft_sfatal(const char *msg, int status)
{
	ft_putendl_fd(msg, STDERR_FILENO);
	_exit(status <= 0 ? 1 : status);
}

inline void	ft_putstr_fds(const char *s, int fd1, int fd2)
{
	ft_putstr_fd(s, fd1);
	ft_putstr_fd(s, fd2);
}

inline void	ft_putendl_fds(const char *s, int fd1, int fd2)
{
	ft_putendl_fd(s, fd1);
	ft_putendl_fd(s, fd2);
}

inline void	ft_mwrites(int fd1, int fd2, const void *buff, size_t nbytes)
{
	(void)write(fd1, buff, nbytes);
	(void)write(fd2, buff, nbytes);
}
