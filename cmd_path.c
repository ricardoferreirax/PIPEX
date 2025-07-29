/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:39:28 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/29 12:41:34 by rmedeiro         ###   ########.fr       */
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
    char *tmp;
    char *fullpath;

    if (!path || !cmd)
        return (NULL);
    tmp = ft_strjoin(path, "/");
    if (!tmp)
        return (NULL);
    fullpath = ft_strjoin(tmp, cmd);
    free(tmp);
    if (!fullpath)
        return (NULL);
    return (fullpath);
}

char *ft_cmd_path(char *cmd, char** envp)
{
    int i;
    char **path_list;
    char *path;
    char *env_path;

    if (!cmd || !envp)
        return (NULL);
    if (access(cmd, X_OK) == 0)
        return (ft_strdup(cmd));
    env_path = ft_env_path(envp);
    if (!env_path)
        return (NULL);
    path_list = ft_split(env_path, ':');
    if (!path_list)
        return (NULL);
    i = 0;
    while (path_list[i])
    {
        path = ft_join_path(path_list[i], cmd);
        if (path && access(path, X_OK) == 0)
            return (ft_free_str(path_list), path);
        free(path);
        i++;
    }
    return (ft_free_str(path_list), NULL);
}
