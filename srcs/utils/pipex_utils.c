/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 00:55:53 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/08 19:55:34 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

void	ft_free_str(char **str)
{
	int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
	{
		free(str[i]);
		str[i] = NULL;
		i++;
	}
	free(str);
}

void	get_pipe_and_fork(int pipefd[2], pid_t *pid)
{
	if (pipe(pipefd) == -1)
		error_exit("Error creating pipe.");
	*pid = fork();
	if (*pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		error_exit("Fork Failed!");
	}
}

void safe_dup2(int oldfd, int newfd)
{
    if (dup2(oldfd, newfd) == -1)
    {
        close(oldfd);
        error_exit("Error. Dup2 failed.");
    }
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
