/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/12 16:23:52 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

static pid_t	safe_fork(char **cmd_args)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("pipex: fork failed");
		ft_free_str(cmd_args);
		exit(1);
	}
	return (pid);
}

static void pipe_process(char **av, int pipefd[2], char **envp)
{
    pid_t pid1;
    pid_t pid2;
    int status[2];

    if (pipe(pipefd) == -1)
        error_exit("Error creating pipe.");
    pid1 = safe_fork(av);
    if (pid1 == 0)
       handle_child(av, pipefd, envp);
    pid2 = safe_fork(av);
    if (pid2 == 0)
        handle_parent(av, pipefd, envp);
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, &status[0], 0);
	waitpid(pid2, &status[1], 0);
    ft_free_str(av);
    if (WIFEXITED(status[1]))
		exit(WEXITSTATUS(status[1]));
	else if (WIFEXITED(status[0]))
		exit(WEXITSTATUS(status[0]));
	return (1);
}

int	main(int ac, char **av, char **envp)
{
	int	pipefd[2];

    if (ac != 5)
    {
        ft_putstr_fd("Input Error!\n", 2); 
        ft_putstr_fd("Use: ./pipex infile cmd1 cmd2 outfile\n", 2);
        exit(EXIT_FAILURE);
    }
    if (av[2][0] == '\0' || av[3][0] == '\0')
        error_exit("Error! Command not valid");
    pipe_process(av, pipefd, envp);
}
