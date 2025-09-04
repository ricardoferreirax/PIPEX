/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/04 09:13:14 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static pid_t	pipe_process(char **av, int pipefd[2], char **envp)
{
	pid_t	pid1;
	pid_t	pid2;

	if (pipe(pipefd) == -1)
		error_exit("Error creating pipe.");
	pid1 = fork();
	if (pid1 < 0)
	{
		perror("pipex: fork failed");
		exit(1);
	}
	if (pid1 == 0)
		handle_first_child(av, pipefd, envp);
	pid2 = fork();
	if (pid2 < 0)
	{
		perror("pipex: fork failed");
		exit(1);
	}
	if (pid2 == 0)
		handle_second_child(av, pipefd, envp);
	close(pipefd[0]);
	close(pipefd[1]);
	return (pid2);
}

int	main(int ac, char **av, char **envp)
{
	int	pipefd[2];
	int	last_pid;
	int	exit_status;

	if (ac != 5)
	{
		ft_putstr_fd("Input Error!\n", 2);
		ft_putstr_fd("Use: ./pipex infile cmd1 cmd2 outfile\n", 2);
		return (EXIT_FAILURE);
	}
	if (av[2][0] == '\0' || av[3][0] == '\0')
		error_exit("Error! Command not valid");
	last_pid = pipe_process(av, pipefd, envp);
	exit_status = wait_processes(last_pid);
	return (exit_status);
}
