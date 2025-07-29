/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:39:28 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/29 16:15:57 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char **ft_extract_env_paths(char **envp)
{
    char **paths;
    char *env_path;
    int i;

    paths = NULL;
    env_path = NULL;
    while (envp && envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
        {
            env_path = envp[i] + 5;
            break;
        }
        i++;
    }
    if (env_path)
    {
        paths = ft_split(env_path, ':');
        if (!paths)
            return (NULL);
    }
    return (paths);
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
    char *entire_path;
    char *env_path;

    if (!cmd || !envp)
        return (NULL);
    if (access(cmd, X_OK) == 0)
        return (ft_strdup(cmd));
    path_list = extract_env_paths(envp);
    if (!path_list)
        return (NULL);
    i = 0;
    while (path_list && path_list[i])
    {
        entire_path = ft_join_path(path_list[i], cmd);
        if (entire_path && access(entire_path, F_OK | X_OK) == 0)
            return (ft_free_str(path_list), entire_path);
        free(entire_path);
        i++;
    }
    ft_free_str(path_list);
    return (NULL);
}
