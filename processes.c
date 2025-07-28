/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/28 14:51:57 by rmedeiro         ###   ########.fr       */
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

int	wait_processes(int last_pid)
{
	int	pid;
	int	status;
	int	last_status;
    
	last_status = 0;
	while ((pid = wait(&status)) > 0)
	{
		if (pid == last_pid && WIFEXITED(status))
			last_status = WEXITSTATUS(status);
	}
	return (last_status);
}
