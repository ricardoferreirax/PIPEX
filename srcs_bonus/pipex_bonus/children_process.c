/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   children_process.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/01 16:12:46 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

static void safe_dup2(int oldfd, int newfd)
{
    if (dup2(oldfd, newfd) == -1)
    {
        close(oldfd);
        error_exit("Error. Dup2 failed.");
    }
}

pid_t	first_child(char *cmd, char **envp)
{
	int	pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
		error_exit("Error creating pipe.");
	pid = fork();
	if (pid < 0)
		error_exit("Pipex: fork failed.");
	if (pid == 0)
    {
        close(pipefd[0]);
        safe_dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        ft_exec_cmd(cmd, envp);
    }
    close(pipefd[1]);
    safe_dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    return (pid);
}

pid_t    middle_child(char *cmd, char **envp)
{
    int     pipefd[2];
    pid_t   pid;

    if (pipe(pipefd) == -1)
        error_exit("Error creating pipe.");
    pid = fork();
    if (pid < 0)
        error_exit("Pipex: fork failed.");
    if (pid == 0)
    {
        close(pipefd[0]);
        safe_dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        ft_exec_cmd(cmd, envp);
    }
    close(pipefd[1]);
    safe_dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    return (pid);
}

pid_t    last_child(char *cmd, char **envp)
{
    pid_t   pid;

    pid = fork();
    if (pid < 0)
        error_exit("Pipex: fork failed.");
    if (pid == 0)
        ft_exec_cmd(cmd, envp);
    return (pid);
}

int	wait_processes(int last_pid)
{
	int	pid;
	int	status;
	int	last_status;

	pid = 1;
	last_status = 0;
	while (pid > 0)
	{
		pid = wait(&status);
		if (pid == last_pid && WIFEXITED(status))
			last_status = WEXITSTATUS(status);
	}
	return (last_status);
}
