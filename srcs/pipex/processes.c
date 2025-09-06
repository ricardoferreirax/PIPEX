/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processes.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/06 17:10:12 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static void	dup2_infile(int infile, int pipe_write)
{
	if (dup2(infile, STDIN_FILENO) == -1)
	{
		close(infile);
		close(pipe_write);
		error_exit("Error. Dup2 failed (stdin)");
	}
	if (dup2(pipe_write, STDOUT_FILENO) == -1)
	{
		close(infile);
		close(pipe_write);
		error_exit("Error. Dup2 failed (stdout)");
	}
}

static void	dup2_outfile(int outfile, int pipe_read)
{
	if (dup2(pipe_read, STDIN_FILENO) == -1)
	{
		close(outfile);
		close(pipe_read);
		error_exit("Error. Dup2 failed (pipefd[0] -> STDIN)");
	}
	if (dup2(outfile, STDOUT_FILENO) == -1)
	{
		close(outfile);
		close(pipe_read);
		error_exit("Error. Dup2 failed (outfile -> STDOUT)");
	}
}

void	handle_first_child(char **av, int pipefd[2], char **envp)
{
	int	infile_fd;

	infile_fd = open(av[1], O_RDONLY);
	if (infile_fd == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		error_exit("Error opening input file");
	}
	close(pipefd[0]);
	dup2_infile(infile_fd, pipefd[1]);
	close(pipefd[1]);
	close(infile_fd);
	ft_exec_cmd(av[2], envp);
}

void	handle_second_child(char **av, int pipefd[2], char **envp)
{
	int	outfile;

	outfile = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		error_exit("Error opening output file");
	}
	close(pipefd[1]);
	dup2_outfile(outfile, pipefd[0]);
	close(pipefd[0]);
	close(outfile);
	ft_exec_cmd(av[3], envp);
}

int	wait_processes(int last_pid)
{
	int	pid;
	int	status;
	int	last_status;

	pid = 1;
	last_status = 0;
	while (pid > 0)
	{
		pid = wait(&status);
		if (pid == last_pid && WIFEXITED(status))
			last_status = WEXITSTATUS(status);
	}
	return (last_status);
}
