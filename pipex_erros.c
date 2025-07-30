/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_erros.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:33:23 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/30 22:46:43 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void    error_exit(const char *message)
{
    perror(message);
    exit(1);
}

void    cmd_notfound(char *cmd)
{
    ft_putstr_fd("pipex: ", 2);
    ft_putstr_fd(cmd, 2);
    ft_putendl_fd(": command not found", 2);
    exit(127);
}

void cmd_not_file_dir(char *cmd)
{
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(": No such file or directory", 2);
	exit(127);
}

void ft_handle_errors(int error)
{
    if (error == 1)
    {
        ft_putstr_fd("Error. Wrong number of arguments\n", 2);
		ft_putstr_fd("Correct usage: ./pipex infile cmd1 cmd2 outfile\n", 2);
    }
    
}