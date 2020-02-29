/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   term.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:46:00 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/29 17:56:12 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/ioctl.h>

#include "ft_script.h"

t_uint8		configure_inherited_tty(t_uint8 reset)
{
	static struct termios	itty_saved;
	struct termios			itty_new;

	if (reset)
		return (ft_tcsetattr(STDIN_FILENO, TCSANOW, &itty_saved) == 0);
	if (ft_tcgetattr(STDIN_FILENO, &itty_saved) != 0)
		return (FALSE);
	itty_new = itty_saved;
	itty_new.c_lflag &= (unsigned long)~(ICANON | ECHO | ISIG);
	return (ft_tcsetattr(STDIN_FILENO, TCSANOW, &itty_new) == 0);
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
