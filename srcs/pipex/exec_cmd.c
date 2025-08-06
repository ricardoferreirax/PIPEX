/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:37:52 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/06 20:49:16 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static char **ft_parse_cmd(char *cmd)
{
    char **cmd_list;

    cmd_list = ft_split_quotes(cmd, ' ');
    if (!cmd_list || !cmd_list[0])
    {
        cmd_not_found_msg(cmd);
        ft_free_str(cmd_list);
        exit(127);
    }
    return (cmd_list);
}

static void check_cmd_access(char *cmd, char **args)
{
    if (access(args[0], F_OK) != 0)
    {
        path_not_found_msg(cmd);
        ft_free_str(args);
        exit(127);
    }
    else if (access(args[0], X_OK) != 0)
    {
        perror("Pipex");
        ft_free_str(args);
        exit(126);
    }
}

void ft_exec_cmd(char *cmd, char **envp)
{
    char **cmd_list;
    char *cmd_path;

    cmd_list = ft_parse_cmd(cmd);
    if (ft_strchr(cmd_list[0], '/'))
		check_cmd_access(cmd, cmd_list);
    cmd_path = ft_cmd_path(cmd_list[0], envp);
    if (!cmd_path)
    {
        cmd_not_found_msg(cmd_list[0]);
        ft_free_str(cmd_list);
        exit(127);
    }
    if (execve(cmd_path, cmd_list, envp) == -1)
	{   
        perror("execve failed");
		ft_free_str(cmd_list);
		free(cmd_path);
		if (errno == EACCES)
            exit(126);
        else if (errno == ENOENT)
            exit(127);
        exit(EXIT_FAILURE);
	}
}
