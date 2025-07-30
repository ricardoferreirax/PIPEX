/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_errors.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:33:23 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/30 23:20:32 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void    error_exit(const char *message)
{
    perror(message);
    exit(1);
}

void	cmd_not_found_free(char *cmd, char **to_free)
{
	ft_free_str(to_free);
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(": command not found", 2);
	exit(127);
}

void	path_not_found_free(char *cmd, char **to_free)
{
	ft_free_str(to_free);
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(": No such file or directory", 2);
	exit(127);
}
