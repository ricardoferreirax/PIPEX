/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:37:52 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/27 17:24:11 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void handle_cmd_with_path(char *cmd, char **args)
{
    if (access(args[0], X_OK) != 0)
    {
        ft_free_str(args);
        cmd_notfound(cmd);
    }
}

void ft_exec_cmd(char *cmd, char **envp)
{
    char **list_cmd;
    char *path;

    if (!cmd || cmd[0] == '\0')
        cmd_notfound(cmd);
    list_cmd = ft_split(cmd, ' ');
    if (!list_cmd || !list_cmd[0])
    {
        ft_free_str(list_cmd);
        cmd_notfound(cmd);
	}
    if (ft_strchr(list_cmd[0], '/'))
		handle_cmd_with_path(list_cmd[0], list_cmd);
    path = ft_cmd_path(list_cmd[0], envp);
    if (!path)
	{
		ft_free_str(list_cmd);
		cmd_notfound(list_cmd[0]);
	}
    execve(path, list_cmd, envp);
    ft_free_str(list_cmd);
    free(path);
    error_exit("execve failed");
}
