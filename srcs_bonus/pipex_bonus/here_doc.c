/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:51:44 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/01 11:18:11 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

int	here_doc(char *limiter)
{
	int		pipe_fd[2];
	char	*line;

	if (pipe(pipe_fd) == -1)
		exit(EXIT_FAILURE);
	if (!fork())
	{
		close(pipe_fd[0]);
		while (1)
		{
			write(1, "heredoc> ", 9);
			line = get_next_line(STDIN_FILENO);
			if (!line || !line[0] || ft_strcmp(line, limiter) - 10 == 0)
			{
				free(line);
				exit(EXIT_SUCCESS);
			}
			write(pipe_fd[1], line, ft_strlen(line));
			free(line);
		}
	}
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	return (wait(NULL), 1);
}
