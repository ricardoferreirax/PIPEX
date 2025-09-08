/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_errors.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:33:23 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/08 16:44:52 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

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

void show_usage_exit2(void)
{
	ft_putstr_fd("================== PIPEX BONUS ====================\n", 2);
	ft_putstr_fd("INPUT ERROR !\n\n", 2);
	ft_putstr_fd("Replicate the Shell pipe (Mandatory):\n", 2);
	ft_putstr_fd("< infile cmd1 | cmd2 > outfile\n\n", 2);
	ft_putstr_fd("< infile cmd1 | cmd2 | ... | cmd[n - 1] > outfile\n\n", 2);
	ft_putstr_fd("Bonus: Insert Two or More Commands:\n", 2);
	ft_putstr_fd("./pipex_bonus infile cmd1 cmd2 ... cmd[n - 1] outfile\n\n", 2);
	ft_putstr_fd("Replicate the Shell here_doc:\n", 2);
	ft_putstr_fd("< here_doc LIMITER cmd1 | cmd2 | ... | cmdN >> outfile\n\n", 2);
	ft_putstr_fd("Format - Insert Two or More Commands:\n", 2);
	ft_putstr_fd("./pipex_bonus here_doc LIMITER cmd1 cmd2 ... cmdN outfile\n", 2);
	ft_putstr_fd("====================================================\n", 2);
	exit(EXIT_FAILURE);
}
