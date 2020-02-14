/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:11:02 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/14 03:00:06 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/ioctl.h>

#include "ft_script.h"

void		sfatal(const char *msg, int status)
{
	ft_putendl_fd(msg, STDERR_FILENO);
	_exit(status <= 0 ? 1 : status);
}

void		ft_bwrite(int fd, const void *data, size_t nbytes, t_uint8 reset)
{
	static unsigned char	buff[512];
	static size_t			bfill;

	if (bfill > 0 && (reset || bfill + nbytes > sizeof(buff)))
	{
		(void)write(fd, buff, bfill);
		if (reset < 2)
		{
			ft_bzero(buff, sizeof(buff));
			bfill = 0;
		}
	}
	if (!data || !nbytes)
		return ;
	if (nbytes > sizeof(buff) - bfill)
	{
		(void)write(fd, data, nbytes);
		return ;
	}
	(void)ft_memcpy(buff + bfill, data, nbytes);
	bfill += nbytes;
}

int			ft_ptsname_r(int fd, char *buff, size_t max)
{
	char	ib[PTSNAME_MAX_SIZE];
	int		tmp;

	if (max == 0)
		return (-1);
	if ((tmp = ioctl(fd, TIOCPTYGNAME, ib)) != 0)
		return (tmp);
	(void)ft_strncpy(buff, ib, max - 1);
	return (0);
}

char		*ft_ptsname(int fd)
{
	static char	buff[PTSNAME_MAX_SIZE];

	if (ft_ptsname_r(fd, buff, PTSNAME_MAX_SIZE) != 0)
		return (NULL);
	return (buff);
}
