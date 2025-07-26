/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:37:52 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/26 15:24:37 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void handle_cmd_with_path(char *cmd, char **args)
{
    if (access(args[0], X_OK) != 0)
    {
        ft_putstr_fd("command not found: ", 2);
        ft_putendl_fd(args[0], 2);
        ft_free_str(args);
        exit(127);
    }
}

void ft_execute_command(char *cmd, char **envp)
{
    char **args;
    char *path;

    args = ft_split(cmd, ' ');
    if (!args || !args[0])
    {
        ft_free_str(args);
		ft_putstr_fd("invalid command: ", 2);
		exit(127);
	}
    path = ft_cmd_path(args[0], envp);
    if (ft_strchr(args[0], '/'))
		handle_cmd_with_path(args[0], envp);
    if (!path)
	{
        ft_putstr_fd("command not found: ", 2);
		ft_putendl_fd(args[0], 2);
		ft_free_str(args);
		exit(127);
	}
    execve(path, args, envp);
    perror("execve failed");
    ft_free_str(args);
    free(path);
    exit(1);
}
