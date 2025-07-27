/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:39:28 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/27 12:37:50 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char *ft_env_path(char **envp)
{
    int i;

    i = 0;
    while (envp && envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
            return (envp[i] + 5);
        i++;
    }
    return (NULL);
}

static char *ft_join_path(char *path, char *cmd)
{
    char * tmp;
    char *fullpath;

    tmp = ft_strjoin(path, "/");
    if (!tmp)
        return (NULL);
    fullpath = ft_strjoin(tmp, cmd);
    if (!fullpath)
        return (NULL);
    free(tmp);
    return (fullpath);
}

char *ft_cmd_path(char *cmd, char** envp)
{
    int i;
    char **list_path;
    char *path;
    char *env_path;

    if (!cmd || !envp)
        return (NULL);
    if (access(cmd, X_OK) == 0)
        return (ft_strdup(cmd));
    env_path = ft_env_path(envp);
    if (!env_path)
        return (NULL);
    list_path = ft_split(env_path, ':');
    if (!list_path)
        return (NULL);
    i = 0;
    while (list_path[i])
    {
        path = ft_join_and_check(list_path[i], cmd);
        if (access(path, X_OK) == 0)
        {
            ft_free_str(list_path);
            return (path);
        }
        free(path);
        i++;
    }
    ft_free_str(list_path);
    free(env_path);
    free(cmd);
    return (NULL);
}
