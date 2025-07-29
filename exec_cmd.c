/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:37:52 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/29 12:38:22 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void check_cmd_access(char *cmd, char **args)
{
    if (access(args[0], F_OK) != 0)
    {
        ft_free_str(args);
        cmd_not_file_dir(cmd);
    }
    else if (access(args[0], X_OK) != 0)
    {
        ft_free_str(args);
        error_exit("Permission denied");
    }
}

void ft_exec_cmd(char *cmd, char **envp)
{
    char **cmd_list;
    char *cmd_path;

    if (!cmd || cmd[0] == '\0')
        cmd_notfound(cmd);
    cmd_list = ft_split(cmd, ' ');
    if (!cmd_list || !cmd_list[0])
    {
        ft_free_str(cmd_list);
        cmd_notfound(cmd);
	}
    if (ft_strchr(cmd_list[0], '/'))
		check_cmd_access(cmd, cmd_list);
    cmd_path = ft_cmd_path(cmd_list[0], envp);
    if (!cmd_path)
	{
        ft_free_str(cmd_list);
        cmd_notfound(cmd_list[0]);
	}
    if (execve(cmd_path, cmd_list, envp) == -1)
	{
		ft_free_str(cmd_list);
		free(cmd_path);
		error_exit("execve failed");
	}
}
