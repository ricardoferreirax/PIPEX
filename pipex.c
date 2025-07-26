/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/26 14:59:57 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void pipe_process(char **av, int pipefd[2], char **envp)
{
    pid_t pid1;
    pid_t pid2;
    
    if (pipe(pipefd) == -1)
    {
        perror("pipe failed");
        exit(1);
    }
    pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork failed");
        exit(1);
    }
    if (pid1 == 0)
       handle_child1(av, pipefd, envp);
    pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork failed");
        exit(1);
    }
    if (pid2 == 0)
        handle_child2(av, pipefd, envp);
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    wait_for_children(pid1, pid2);
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
