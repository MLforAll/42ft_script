/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   play.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/09 02:54:55 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/09 04:44:34 by kdumarai         ###   ########.fr       */
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
	ptr->hdr = (t_rts_hdr *)ptr->map;
	return (TRUE);
}

inline static void	records_sleep(t_rts_record *curr, t_rts_record *next)
{
	struct timespec		rq;

	rq.tv_sec = next->timestamp - curr->timestamp;
	if (curr->ntimestamp < next->ntimestamp)
		rq.tv_nsec = (next->ntimestamp - curr->ntimestamp) * 1000;
	else
		rq.tv_nsec = (curr->ntimestamp - next->ntimestamp) * 1000;
	(void)nanosleep(&rq, NULL);
}

int					play_file(t_typescript *ts, t_opts *opts)
{
	t_rts			rts;
	t_rts_record	*r;
	t_rts_record	*nr;
	struct iovec	rs;

	if (!map_file(&rts, ts))
		return (EXIT_FAILURE);
	r = &rts.hdr->initial_record;
	nr = (t_rts_record *)((t_uintptr)rts.map + sizeof(t_rts_hdr));
	while (r && nr)
	{
		if (r->direction == 'o' && get_record_string(&rts, r, &rs))
			(void)write(STDOUT_FILENO, rs.iov_base, rs.iov_len);
		if (!(opts->switches & kSwitchD))
			records_sleep(r, nr);
		r = nr;
		nr = next_record(&rts, r);
	}
	(void)munmap(rts.map, rts.sz);
	return (EXIT_SUCCESS);
}
