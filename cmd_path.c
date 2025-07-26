/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:39:28 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/26 14:55:05 by rmedeiro         ###   ########.fr       */
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

static char *ft_join_and_check(char *path, char *cmd)
{
    char * tmp;
    char *fullpath;

    tmp = ft_strjoin(path, "/");
    if (!tmp)
        return (NULL);
    fullpath = ft_strjoin(tmp, cmd);
    free(tmp);
    if (!fullpath)
        return (NULL);
    if (access(fullpath, X_OK) == 0) // check if the command is executable
        return (fullpath); // return the full path to the command
    free(fullpath);
    return (NULL);
}

char *ft_cmd_path(char *cmd, char** envp)
{
    int i;
    char **paths;
    char *fullpath;
    char *env_path;

    env_path = ft_env_path(envp);
    if (!env_path)
        return (NULL);
    if (access(cmd, X_OK) == 0) // check if the command is executable
        return (ft_strdup(cmd));
    paths = ft_split(env_path, ':'); // split PATH into directories
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        fullpath = ft_join_and_check(paths[i], cmd);
        if (fullpath)
            return (ft_free_str(paths), fullpath);
        i++;
    }
    ft_free_str(paths);
    return (NULL);
}
