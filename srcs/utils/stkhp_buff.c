/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stkhp_buff.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/14 05:28:57 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/29 21:40:41 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include "ft_script.h"

void	*stkhp_buff_alloc(t_stkhp_buff *out, size_t sz)
{
	ft_bzero(out->r_stk, sizeof(out->r_stk));
	if (sz <= sizeof(out->r_stk))
		return ((out->ptr = (void *)out->r_stk));
	out->r_map_ptr = mmap(NULL, sz, PROT_READ | PROT_WRITE, \
		MAP_ANON | MAP_PRIVATE, -1, 0);
	if (!out->r_map_ptr)
		return (NULL);
	out->r_map_sz = sz;
	return ((out->ptr = out->r_map_ptr));
}

void	stkhp_buff_free(t_stkhp_buff *buff)
{
	if (buff->r_map_ptr)
		(void)munmap(buff->r_map_ptr, buff->r_map_sz);
	buff->r_map_ptr = NULL;
	buff->r_map_sz = 0;
	buff->ptr = NULL;
}
