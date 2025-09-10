/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_errors.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:33:23 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/10 18:28:40 by rmedeiro         ###   ########.fr       */
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

void warn_heredoc_eof(char *lim)
{
    write(STDERR_FILENO,
          "warning: here-document delimited by end-of-file (wanted '", 58);
    write(STDERR_FILENO, lim, ft_strlen(lim));
    write(STDERR_FILENO, "')\n", 3);
}

void show_usage_exit2(void)
{
	ft_putstr_fd("================== PIPEX BONUS =========================", 2);
	ft_putstr_fd("========\nINPUT ERROR! Not Enough Arguments!\n\n", 2);
	ft_putstr_fd("Replicate the Shell pipe:\n\n", 2);
	ft_putstr_fd("This: ./pipex_bonus cmd1 cmd2 ... cdmN outfile\n\n", 2);
	ft_putstr_fd("Should behave like: < infile cmd1 | cmd2 | ... | cmdN >", 2);
	ft_putstr_fd(" outfile\n\nReplicate the Shell heredoc:\n\n", 2);
	ft_putstr_fd("This: ./pipex_bonus here_doc LIMITER cmd cmd1 outfile\n\n", 2);
	ft_putstr_fd("Should Behave Like: cmd1 << LIMITER | cmd2 | ... | cmdN >>", 2);
	ft_putstr_fd(" outfile\n", 2);
	ft_putstr_fd("=========================================================", 2);
	ft_putstr_fd("=========", 2);
	exit(EXIT_FAILURE);
}
