/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/25 15:44:23 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void execute_command(char *cmd, char **envp)
{
    char **args;
    char *path;

    args = ft_split(cmd, ' '); // split the command into arguments
    if (!args || !args[0])
    {
		ft_printf("Invalid command\n");
		exit(1);
	}
    path = find_path(); // find the path of the command
    if (!path)
	{
		ft_putstr_fd("command not found: ", 2);
		ft_putendl_fd(args[0], 2);
		free_split(args);
		exit(127);
	}
    exceve(path, args, envp); // execute the command with the environment variables
    perror("execve failed"); // if execve fails, print error message
    free_split(args); // free the arguments
    free(path);
    exit(1); // exit if execve fails
}

void cmd1_process(char **av, int pipefd[2], char **envp)
{
    int fd;

    fd = open(av[1], O_RDONLY); // open file1 (av[1]) for reading
    dup2(fd, STDIN_FILENO); // redirect input from file1 (fd) -> stdin: fd ()
    dup2(pipefd[1], STDOUT_FILENO); // redirect output to pipe (pipefd[1]) -> stdout: pipefd[1] (write on pipe)
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // cclose write end of pipe (pipefd[1])
    close(fd); // close file descriptor for file1 (close input_fd)
    execute_command(av[2], envp); // execute the command (cmd)
}

void cmd2_process(char **av, int pipefd[2], char **envp)
{
    int fd;

    fd = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644); // open file2 (av[4]) for writing
    dup2(pipefd[0], STDIN_FILENO); // redirect input from pipe (pipefd[0]) -> stdin: pipefd[0] (read from pipe)
    dup2(fd, STDOUT_FILENO); // redirect output to file2 (fd) -> stdout: fd (write on file2)
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(fd); // close file descriptor for file2 (close output_fd)
    execute_command(av[3], envp); // execute the command (cmd)
}

/* void	pipe_process(int p_fd[2], int fd1, int fd2, char **av, char **envp)
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
} */

#include <sys/types.h>

int main(int ac, char **av, char **envp)
{
    int pipefd[2];
    pid_t pid;

    if (ac != 5)
    {
        write(2, "Usage: ./pipex file1 cmd1 cmd2 file2\n", 37);
        exit (1);
    }
    if (pipe(pipefd) == -1)
    {
        perror("...");
        exit (1);
    }
    pid = fork();
    if (pid == -1)
    {
        perror("...");
        exit (1);
    }
    if (!pid)
        cmd1_process(av, pipefd, envp);
    else
        cmd2_process(av, pipefd, envp);
}