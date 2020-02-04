/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   term.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/04 05:46:00 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/04 06:15:57 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "ft_script.h"

t_uint8		configure_inherited_tty(t_uint8 reset)
{
	static struct termios	itty_saved;
	struct termios			itty_new;

	if (reset)
		return (tcsetattr(STDIN_FILENO, TCSANOW, &itty_saved) == 0);
	if (ft_tcgetattr(STDIN_FILENO, &itty_saved) != 0)
		return (FALSE);
	itty_new = itty_saved;
	itty_new.c_lflag &= ~(ICANON | ECHO | ISIG);
	return (tcsetattr(STDIN_FILENO, TCSANOW, &itty_new) == 0);
}
