/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processes.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/12 16:03:55 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static void dup2_infile(int infile, int pipe_write)
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

static void dup2_outfile(int outfile, int pipe_read)
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

void handle_child(char **av, int pipefd[2], char **envp)
{
    int infile;

    infile = open(av[1], O_RDONLY);
    if (infile == -1)
    {
		close(pipefd[0]);
        close(pipefd[1]);
		error_exit("Error opening input file");
    }
    close(pipefd[0]);
    dup2_infile(infile, pipefd[1]);
    close(pipefd[1]);
    close(infile);
    ft_exec_cmd(av[2], envp);
}

void handle_parent(char **av, int pipefd[2], char **envp)
{
    int outfile;

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

void wait_processes(pid_t pid1, pid_t pid2)
{
    int	status1;
	int	status2;
	int	exit_status;
    exit_status = 0;
    
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	if (WIFEXITED(status1))
		exit_status = WEXITSTATUS(status1);
	if (WIFEXITED(status2))
		exit_status = WEXITSTATUS(status2);
	exit(exit_status);
}
