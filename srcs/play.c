/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   play.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/09 02:54:55 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/12 03:05:25 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "ft_script.h"

static t_uint8				map_file(t_rts *ptr, t_typescript *ts)
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
	ptr->firstr = (t_rts_record *)ptr->map;
	if (!ft_memfitsinbuff(NULL, ptr->firstr, sizeof(t_rts_record), ptr->endptr))
	{
		(void)munmap(ptr->map, sz);
		return (FALSE);
	}
	return (TRUE);
}

static void					records_sleep(t_rts_record *r, t_rts_record *nr)
{
	struct timespec		rq;
	t_uint32			udiff;

	if ((udiff = nr->utimestamp) < r->utimestamp)
	{
		rq.tv_sec = 0;
		udiff = udiff + 1000000 - r->utimestamp;
	}
	else
	{
		rq.tv_sec = nr->timestamp - r->timestamp;
		udiff = nr->utimestamp - r->utimestamp;
	}
	rq.tv_nsec = udiff * 1000;
	(void)nanosleep(&rq, NULL);
}

inline static void			print_record_data(t_rts_record *r)
{
	const char	*data;

	if (r->size == 0)
		return ;
	data = (const char *)((t_uintptr)r + sizeof(t_rts_record));
	(void)write(STDOUT_FILENO, data, r->size);
}

inline static t_rts_record	*next_record(t_rts *rts, \
										t_rts_record *r, \
										enum e_rts_direction not_dir)
{
	while (TRUE)
	{
		r = (t_rts_record *)((t_uintptr)r + sizeof(t_rts_record) + r->size);
		if (!ft_memfitsinbuff(NULL, r, sizeof(t_rts_record), rts->endptr))
			return (NULL);
		if (not_dir == kDirectionAll || (char)r->direction != (char)not_dir)
			break ;
	}
	return (r);
}

int							play_file(t_typescript *ts, t_opts *opts)
{
	t_rts			rts;
	t_rts_record	*r;
	t_rts_record	*nr;

	if (!map_file(&rts, ts))
		return (EXIT_FAILURE);
	r = rts.firstr;
	while (TRUE)
	{
		if (r->direction == kDirectionStart && !(opts->switches & kSwitchQ))
			announce_script_time(STDOUT_FILENO, (time_t)r->timestamp, NO, YES);
		if (r->direction == kDirectionOutput)
			print_record_data(r);
		if (r->direction == kDirectionEnd && !(opts->switches & kSwitchQ))
			announce_script_time(STDOUT_FILENO, (time_t)r->timestamp, NO, NO);
		if (!(nr = next_record(&rts, r, kDirectionInput)))
			break ;
		if (!(opts->switches & kSwitchD) && nr->direction != kDirectionStart)
			records_sleep(r, nr);
		r = nr;
	}
	(void)munmap(rts.map, rts.sz);
	return (EXIT_SUCCESS);
}
