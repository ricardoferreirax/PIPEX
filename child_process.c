/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/27 16:52:42 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void handle_child(char **av, int pipefd[2], char **envp)
{
    int infile;

    infile = open(av[1], O_RDONLY); // open file1 (av[1]) for reading
    if (infile == -1)
    {
		close(pipefd[1]);
		error_exit("error opening input file");
    }
    if (dup2(infile, STDIN_FILENO) == -1) // redirect input from file1 (infile) -> stdin: infile | verify if dup2 was successful
    {
        close(infile);
        close(pipefd[1]);
        error_exit("dup2 failed (infile -> STDIN)");
    }
    if (dup2(pipefd[1], STDOUT_FILENO) == -1) // redirect output to pipe (pipefd[1]) -> stdout: pipefd[1] (write on pipe) | verify if dup2 was successful
    {
        close(infile);
        close(pipefd[1]);
        error_exit("dup2 failed (pipefd[1] -> STDOUT)");
    }
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(infile); // close file descriptor for file1 (close infile)
    ft_exec_cmd(av[2], envp); // execute the command (cmd)
}

void handle_parent(char **av, int pipefd[2], char **envp)
{
    int outfile;

    outfile = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644); // open file2 (av[4]) for writing
    if (outfile == -1)
    {
        close(pipefd[0]);
		error_exit("error opening output file");
    }
    if (dup2(pipefd[0], STDIN_FILENO) == -1) // redirect input from pipe (pipefd[0]) -> stdin: pipefd[0] (read from pipe) | verify if dup2 was successful
    {
        close(outfile);
        close(pipefd[0]);
        error_exit("dup2 failed (pipefd[0] -> STDIN)");
    }
    if (dup2(outfile, STDOUT_FILENO) == -1) // redirect output to file2 (outfile) -> stdout: outfile (write on file2) | verify if dup2 was successful
    {
        close(outfile);
        close(pipefd[0]);
        error_exit("dup2 failed (outfile -> STDOUT)");
    }
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(outfile); // close file descriptor for file2 (close output_fd)
    ft_exec_cmd(av[3], envp); // execute the command (cmd)
}

void wait_for_children(pid_t pid1, pid_t pid2)
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
