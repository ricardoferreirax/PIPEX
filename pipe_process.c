/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/25 15:11:20 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>

void cmd1_process(int pipefd[2], int input_fd, char *cmd, char **envp)
{
    dup2(input_fd, STDIN_FILENO); // redirect input from file1 (input_fd) -> stdin: input_fd ()
    dup2(pipefd[1], STDOUT_FILENO); // redirect output to pipe (pipefd[1]) -> stdout: pipefd[1] (write on pipe)
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // cclose write end of pipe (pipefd[1])
    close(input_fd); // close file descriptor for file1 (close input_fd)
    execute_command(cmd, envp); // execute the command (cmd)
}

void cmd2_process(int pipefd[2], int output_fd, char *cmd, char **envp)
{
    dup2(pipefd[0], STDIN_FILENO); // redirect input from pipe (pipefd[0]) -> stdin: pipefd[0] (read from pipe)
    dup2(output_fd, STDOUT_FILENO); // redirect output to file2 (output_fd) -> stdout: output_fd (write on file2)
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(output_fd); // close file descriptor for file2 (close output_fd)
    execute_command(cmd, envp); // execute the command (cmd)
}

void	pipe_process(int p_fd[2], int fd1, int fd2, char **av, char **envp)
{
	pid_t	pid1;
	pid_t	pid2;

	pid1 = fork();
	if (pid1 == -1)
	{
		perror("fork 1 fail");
		exit(1);
	}
	if (pid1 == 0)
	{
		cmd1_process(p_fd, fd1, av[2], envp);
		exit(1);
	}
	pid2 = fork();
	if (pid2 == -1)
	{
		perror("fork 2 fail");
		exit(1);
	}
	if (pid2 == 0)
	{
		cmd2_process(p_fd, fd2, av[3], envp);
		exit(1);
	}
	close(p_fd[0]);
	close(p_fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}
