/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:11:02 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/04 06:26:15 by kdumarai         ###   ########.fr       */
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
	int	req;

	if (opts & TCSANOW)
		req = TIOCSETA;
	else if (opts & TCSADRAIN)
		req = TIOCSETAW;
	else if (opts & TCSAFLUSH)
		req = TIOCSETAF;
	else
		return (-1);
	return (ioctl(fd, req, t));
}

void		ft_sfatal(const char *msg, int status)
{
	ft_putendl_fd(msg, STDERR_FILENO);
	_exit(status <= 0 ? 1 : status);
}
