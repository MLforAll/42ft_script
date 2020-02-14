/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/05 23:57:22 by kdumarai          #+#    #+#             */
/*   Updated: 2020/02/14 03:35:36 by kdumarai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
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
	return (cmd);
}

#else

static const char			*cmd_path_pathenv(char *pathenv, const char *cmd)
{
	(void)pathenv;
	return (cmd);
}

#endif

void						cmd_init(t_cmd *ptr, const char **av)
{
	char	*tmp;

	if (!av)
	{
		tmp = ft_getenv("SHELL");
		ptr->path = (tmp) ? tmp : "/bin/sh";
		ptr->shargs[0] = (char *)(uintptr_t)ptr->path;
		ptr->shargs[1] = "-i";
		ptr->shargs[2] = NULL;
		ptr->args = ptr->shargs;
		ptr->shell = YES;
		return ;
	}
	if (USE_LIBC_DIR && (tmp = ft_getenv("PATH")))
	{
		ptr->path = cmd_path_pathenv(tmp, *av);
		ptr->runenv = NO;
	}
	else
	{
		ptr->path = *av;
		ptr->runenv = access(*av, X_OK) == 0 ? NO : YES;
	}
	ptr->args = (char **)(uintptr_t)av;
	ptr->shell = NO;
}
