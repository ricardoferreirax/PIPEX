/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processes.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/09 19:32:24 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

void	first_child(char **av, int *prev_read_fd, char **envp)
{
	int	infile_fd;
	int pipefd[2];
	pid_t pid;
	
	get_pipe_and_fork(pipefd, &pid);
	if (pid == 0)
	{
		close(pipefd[0]);
		infile_fd = open(av[1], O_RDONLY);
		if (infile_fd == -1)
		{
			close(pipefd[1]);
			error_exit("Error Opening Input File!");
		}
		safe_dup2(infile_fd, STDIN_FILENO);
		safe_dup2(pipefd[1], STDOUT_FILENO);
	    close(pipefd[1]);
	    close(infile_fd);
	    ft_exec_cmd(av[2], envp);
	}
	close(pipefd[1]);
	*prev_read_fd = pipefd[0];
}

pid_t	last_child(int ac, char **av, int prev_read_fd, char **envp)
{
	int	outfile_fd;
	pid_t pid;

	pid = fork();
	if (pid < 0)
		error_exit("Fork Failed!");
	if (pid == 0)
	{
		outfile_fd = open(av[ac - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (outfile_fd == -1)
		{
			close(prev_read_fd);
			error_exit("Error on output file");
		}
		safe_dup2(prev_read_fd, STDIN_FILENO);
		safe_dup2(outfile_fd, STDOUT_FILENO);
		close(prev_read_fd);
	    close(outfile_fd);
	    ft_exec_cmd(av[ac - 2], envp);
	}
	close(prev_read_fd);
	return (pid);
}
