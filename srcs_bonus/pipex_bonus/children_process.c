/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   children_process.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/13 15:34:54 by rmedeiro         ###   ########.fr       */
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

pid_t	children_process(char *cmd, char **envp, int last)
{
	int		pipefd[2];
	pid_t	pid;

	if (!last && pipe(pipefd) == -1)
		error_exit("Error creating pipe.");
	pid = fork();
	if (pid < 0)
        error_exit("Pipex: fork failed.");
	if (pid == 0)
	{
		if (!last)
        {   
            close(pipefd[0]);
            safe_dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        }
		ft_exec_cmd(cmd, envp);
	}
	if (!last)
    {
        close(pipefd[1]);
        safe_dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
    }
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
