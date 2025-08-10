/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_path_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 22:45:19 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/10 22:48:12 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

static char **ft_extract_env_paths(char **envp)
{
    char **paths;
    char *env_path;
    int i;

    i = 0;
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
        paths = ft_split(env_path, ':');
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
    return (fullpath);
}

static char *ft_check_direct_cmd(char *cmd)
{
    char *dup;
    
    if (ft_strchr(cmd, '/'))
    {
        if (access(cmd, F_OK) != 0)
        {
            path_not_found_msg(cmd);
            return (NULL);
        }
        if (access(cmd, X_OK) == 0)
        {
            dup = ft_strdup(cmd);
            if (!dup)
                error_exit("dup failed");
            return(dup);
        }
        else
            error_exit("Permission denied");
    }
    return (NULL);
}

char *ft_cmd_path(char *cmd, char** envp)
{
    char **path_list;
    char *entire_path;
    char *direct_cmd;
    int i;

    if (!cmd || !envp)
        return (NULL);
    direct_cmd = ft_check_direct_cmd(cmd);
    if (direct_cmd)
        return (direct_cmd);
    path_list = ft_extract_env_paths(envp);
    if (!path_list)
        return (NULL);
    i = 0;
    while (path_list[i])
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
