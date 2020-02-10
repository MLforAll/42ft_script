/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   play.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/09 02:54:55 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/10 01:07:50 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>

#include "ft_script.h"

static t_uint8		map_file(t_rts *ptr, t_typescript *ts)
{
	struct stat	st;
	size_t		sz;

	if (fstat(ts->fd, &st) != 0)
		return (FALSE);
	sz = st.st_size;
	ptr->map = mmap(NULL, sz, PROT_READ, MAP_FILE | MAP_PRIVATE, ts->fd, 0);
	if (!ptr->map)
		return (FALSE);
	ptr->sz = sz;
	ptr->endptr = (void *)((t_uintptr)ptr->map + ptr->sz);
	ptr->initial_record = (t_rts_record *)ptr->map;
	return (TRUE);
}

inline static void	records_sleep(t_rts_record *curr, t_rts_record *next)
{
	struct timespec		rq;

	if (next->timestamp < curr->timestamp)
		rq.tv_sec = curr->timestamp - next->timestamp;
	else
		rq.tv_sec = next->timestamp - curr->timestamp;
	if (curr->ntimestamp < next->ntimestamp)
		rq.tv_nsec = (next->ntimestamp - curr->ntimestamp) * 1000;
	else
		rq.tv_nsec = (curr->ntimestamp - next->ntimestamp) * 1000;
	(void)nanosleep(&rq, NULL);
}

static const char	*get_record_data(t_rts_record *r)
{
	if (r->size)
		return ((const char *)((t_uintptr)r + sizeof(t_rts_record)));
	return (NULL);
}

static t_rts_record	*next_record(t_rts *rts, \
									t_rts_record *r, \
									enum e_rts_direction dir)
{
	while (TRUE)
	{
		r = (t_rts_record *)((t_uintptr)r + sizeof(t_rts_record) + r->size);
		if (!ft_memfitsinbuff(NULL, r, sizeof(t_rts_record), rts->endptr))
			return (NULL);
		if (dir == kDirectionAll || (char)r->direction == (char)dir)
			break ;
	}
	return (r);
}

int					play_file(t_typescript *ts, t_opts *opts)
{
	t_rts			rts;
	t_rts_record	*r;
	t_rts_record	*nr;
	const char		*data;

	if (!map_file(&rts, ts))
		return (EXIT_FAILURE);
	r = rts.initial_record;
	while ((nr = next_record(&rts, r, kDirectionAll)))
	{
		if (r->direction == kDirectionStart)
			announce_script_time(STDOUT_FILENO, (time_t)r->timestamp, NO, YES);
		if (r->direction == kDirectionOutput && (data = get_record_data(r)))
			(void)write(STDOUT_FILENO, data, r->size);
		if (!(opts->switches & kSwitchD))
			records_sleep(r, nr);
		r = nr;
	}
	announce_script_time(STDOUT_FILENO, (time_t)r->timestamp, NO, NO);
	(void)munmap(rts.map, rts.sz);
	return (EXIT_SUCCESS);
}
