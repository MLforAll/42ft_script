/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   play_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/14 02:16:40 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/21 08:20:22 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"

static t_uint8	check_direction(t_rts_record *r)
{
	t_uint8						idx;
	const enum e_rts_direction	dirs[] = {
		kDirectionStart, kDirectionInput, kDirectionOutput, kDirectionEnd
	};

	idx = 0;
	while (idx < sizeof(dirs) / sizeof(*dirs))
		if ((enum e_rts_direction)r->direction == dirs[idx++])
			return (TRUE);
	return (FALSE);
}

int				next_record(t_rts_record **out, \
							t_rts *rts, \
							t_rts_record *r, \
							enum e_rts_direction not_dir)
{
	void	*tmp;

	while (TRUE)
	{
		tmp = (void *)((t_uintptr)r + sizeof(t_rts_record));
		if (tmp == rts->endptr)
		{
			if (out)
				*out = NULL;
			return (TRUE);
		}
		r = (t_rts_record *)((t_uintptr)tmp + r->size);
		if (!ft_memfitsinbuff(NULL, r, sizeof(t_rts_record), rts->endptr))
			return (ft_returnmsg("ft_script: invalid stamp", 2, FALSE));
		if (!check_direction(r))
			return (ft_returnmsg("ft_script: invalid direction", 2, FALSE));
		if (not_dir == kDirectionAll || (char)r->direction != (char)not_dir)
			break ;
	}
	if (out)
		*out = r;
	return (TRUE);
}
