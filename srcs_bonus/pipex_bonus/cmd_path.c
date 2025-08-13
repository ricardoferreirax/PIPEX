/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:39:28 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/13 14:24:10 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static char **ft_get_env_paths(char **envp)
{
    char **paths;
    char *env_path;
    int i;

    if (!envp)
        return (NULL);
    i = 0;
    paths = NULL;
    env_path = NULL;
    while (envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
        {
            env_path = envp[i] + 5;
            break;
        }
        i++;
    }
    if (!env_path)
       return (NULL);
    paths = ft_split(env_path, ':');
    if (!paths)
        exit(1);
    return (paths);
}

static char *ft_join_dir_cmd(char *dir, char *cmd)
{
    char *dir_slash;
    char *fullpath;

    if (!dir || !cmd)
        return (NULL);
    dir_slash = ft_strjoin(dir, "/");
    if (!dir_slash)
        return (NULL);
    fullpath = ft_strjoin(dir_slash, cmd);
    free(dir_slash);
    if (!fullpath)
        return (NULL);
    return (fullpath);
}

char *ft_cmd_path(char *cmd, char** envp)
{
    char **paths;
    char *fullpath;
    int i;

    if (!cmd || !envp)
        return (NULL);
    if (strchr(cmd, '/'))
        return (ft_strdup(cmd));
    paths = ft_get_env_paths(envp);
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        fullpath = ft_join_dir_cmd(paths[i], cmd);
        if (fullpath && access(fullpath, F_OK | X_OK) == 0)
            return (ft_free_str(paths), fullpath);
        free(fullpath);
        i++;
    }
    ft_free_str(paths);
    return (NULL);
}
