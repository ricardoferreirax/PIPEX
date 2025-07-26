/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:37:52 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/26 14:38:11 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void ft_execute_command(char *cmd, char **envp)
{
    char **args;
    char *path;

    args = ft_split(cmd, ' '); // split the command into arguments
    if (!args || !args[0])
    {
        ft_free_str(args);
		ft_putstr_fd("invalid command: ", 2);
		exit(1);
	}
    path = ft_cmd_path(args[0], envp); // get the full path of the command
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
