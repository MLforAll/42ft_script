/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libc_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/06 00:08:44 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/06 00:09:25 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/ioctl.h>

#include "ft_script.h"

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

inline int	ft_tcgetattr(int fd, struct termios *t)
{
	return (ioctl(fd, TIOCGETA, t));
}

inline int	ft_tcsetattr(int fd, int opts, const struct termios *t)
{
	unsigned long	req;

	if (opts == TCSANOW)
		req = TIOCSETA;
	else if (opts == TCSADRAIN)
		req = TIOCSETAW;
	else if (opts == TCSAFLUSH)
		req = TIOCSETAF;
	else
		return (-1);
	return (ioctl(fd, req, t));
}
