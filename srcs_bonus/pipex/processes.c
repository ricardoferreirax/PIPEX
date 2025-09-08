/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processes.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/08 19:47:43 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

void	first_child(char **av, int *prev_readfd, char **envp)
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
	*prev_readfd = pipefd[0];
}

pid_t	middle_child(char **av, int *prev_readfd, char **envp, int j)
{
	int in_fd;
	int pipefd[2];
	pid_t pid;

	in_fd = *prev_readfd;
	get_pipe_and_fork(pipefd, &pid);
	if (pid == 0)
	{
		close(pipefd[0]);
		safe_dup2(in_fd, STDIN_FILENO);
		safe_dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		close(in_fd);
		ft_exec_cmd(av[j], envp);
	}
	close(in_fd);
	close(pipefd[1]);
	*prev_readfd = pipefd[0];
	return (pid);
}

pid_t	last_child(int ac, char **av, int prev_readfd, char **envp)
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
			close(prev_readfd);
			error_exit("Error on output file");
		}
		safe_dup2(prev_readfd, STDIN_FILENO);
		safe_dup2(outfile_fd, STDOUT_FILENO);
		close(prev_readfd);
	    close(outfile_fd);
	    ft_exec_cmd(av[ac - 2], envp);
	}
	close(prev_readfd);
	return (pid);
}

