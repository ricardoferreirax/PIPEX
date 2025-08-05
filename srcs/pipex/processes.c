/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processes.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/05 15:05:05 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

int open_infile(char *file)
{
    int fd;

    fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening input file");
        fd = open("/dev/null", O_RDONLY);
        if (fd == -1)
            error_exit("Failed to open /dev/null");
    }
    return (fd);
}

void handle_child(char **av, int pipefd[2], char **envp)
{
    int infile;

    infile = open_infile(av[1]);
    if (infile == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        error_exit("Error opening input file");
    }
    if (dup2(infile, STDIN_FILENO) == -1)
    {
        close(infile);
        close(pipefd[1]);
        error_exit("Error. Dup2 failed");
    }
    if (dup2(pipefd[1], STDOUT_FILENO) == -1)
    {
        close(infile);
        close(pipefd[1]);
        error_exit("Error. Dup2 failed");
    }
    close(pipefd[0]);
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
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
    {
        close(outfile);
        close(pipefd[0]);
        error_exit("Error. Dup2 failed (pipefd[0] -> STDIN)");
    }
    if (dup2(outfile, STDOUT_FILENO) == -1)
    {
        close(outfile);
        close(pipefd[0]);
        error_exit("Error. Dup2 failed (outfile -> STDOUT)");
    }
    close(pipefd[1]);
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
