/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/30 22:48:17 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void pipe_process(char **av, int pipefd[2], char **envp)
{
    pid_t pid1;
    pid_t pid2;

    if (pipe(pipefd) == -1)
        error_exit("Error. Pipe failed.");
    pid1 = fork();
    if (pid1 < 0)
        error_exit("Error. Fork failed.");
    if (pid1 == 0)
       handle_child(av, pipefd, envp);
    pid2 = fork();
    if (pid2 < 0)
        error_exit("Error. Fork failed.");
    if (pid2 == 0)
        handle_parent(av, pipefd, envp);
    close(pipefd[0]);
    close(pipefd[1]);
    wait_processes(pid1, pid2);
}

int	main(int ac, char **av, char **envp)
{
	int	pipefd[2];

	if (ac == 5)
		pipe_process(av, pipefd, envp);
	else
	{
		ft_putstr_fd("Invalid input. Try: ./pipex file1 cmd1 cmd2 file2\n", 2);
		exit(1);
	}
	return (0);
}
