/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdpath.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/05 23:57:22 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/06 00:07:24 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"

#if USE_LIBC_DIR

# include <dirent.h>

inline static const char	*cmd_path_build(const char *dir, const char *base)
{
	static char		cmdpathbuff[__DARWIN_MAXPATHLEN];

	(void)ft_strcpy(cmdpathbuff, dir);
	(void)ft_strcat(cmdpathbuff, "/");
	(void)ft_strcat(cmdpathbuff, base);
	return (cmdpathbuff);
}

static const char			*cmd_path_pathenv(char *pathenv, const char *cmd)
{
	char			*tmp;
	DIR				*dirp;
	struct dirent	*entry;

	while ((tmp = ft_strchr(pathenv, ':')))
	{
		pathenv[tmp - pathenv] = '\0';
		if ((dirp = opendir(pathenv)))
		{
			while ((entry = readdir(dirp)))
				if (ft_strequ(entry->d_name, cmd))
					return (cmd_path_build(pathenv, cmd));
			(void)closedir(dirp);
		}
		pathenv[tmp - pathenv] = ':';
		pathenv = tmp + 1;
	}
	return (NULL);
}

#else

static const char			*cmd_path_pathenv(char *pathenv, const char *cmd)
{
	(void)pathenv;
	return (cmd);
}

#endif

const char					*cmd_path(const char *cmd)
{
	char	*tmp;

	if (!cmd)
	{
		tmp = ft_getenv("SHELL");
		return (tmp ? tmp : "/bin/sh");
	}
	if (USE_LIBC_DIR && (tmp = ft_getenv("PATH")))
		return (cmd_path_pathenv(tmp, cmd));
	return (cmd);
}
