/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_errors.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:33:23 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/10 18:27:08 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

void	error_exit(const char *message)
{
	perror(message);
	exit(1);
}

void	cmd_not_found_msg(char *cmd)
{
	ft_putstr_fd("Pipex: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(": command not found", 2);
}

void	path_not_found_msg(char *cmd)
{
	ft_putstr_fd("Pipex: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(": No such file or directory", 2);
}

void show_usage_exit1(void)
{
	ft_putstr_fd("================== PIPEX MANDATORY ==================\n", 2);
	ft_putstr_fd("INPUT ERROR! Not Enough Arguments\n\n", 2);
	ft_putstr_fd("Replicate the Shell pipe:\n\n", 2);
	ft_putstr_fd("This: ./pipex infile cmd1 cmd1 outfile\n\n", 2);
	ft_putstr_fd("Should Behave Like: < infile cmd1 | cmd2 > outfile\n", 2);
	ft_putstr_fd("====================================================\n", 2);
	exit(EXIT_FAILURE);
}
