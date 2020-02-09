/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   record_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/09 04:28:13 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/09 04:33:22 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "ft_script.h"

t_rts_record	*next_record(t_rts *rts, t_rts_record *r)
{
	char			*s;
	t_rts_record	*ret;

	s = (char *)&r->start;
	while (*s != 0x1 && *s != 0x2)
	{
		if ((void *)s > rts->endptr)
			return (NULL);
		s++;
	}
	ret = (t_rts_record *)(s + sizeof(r->start));
	return ((void *)ret > rts->endptr ? NULL : ret);
}

t_uint8			get_record_string(t_rts *rts, \
									t_rts_record *r, \
									struct iovec *ptr)
{
	char	*s;
	size_t	len;

	s = (char *)&r->start;
	len = 0;
	while (s[len] != 0x1 && s[len] != 0x2)
	{
		if ((void *)(s + len) > rts->endptr)
			return (FALSE);
		len++;
	}
	if (ptr)
	{
		ptr->iov_base = s;
		ptr->iov_len = len;
	}
	else
		(void)write(STDOUT_FILENO, s, len);
	return (TRUE);
}
