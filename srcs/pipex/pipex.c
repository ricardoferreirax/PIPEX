/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/08 02:30:15 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

int main(int ac, char **av, char **envp)
{
	int oldfd;
	int last_pid;
	int exit_code;
	int i;

	if (ac < 5)
		show_usage_exit();
	if (ft_strncmp(av[1], "here_doc", 8) != 0)
	{
		first_child(av, &oldfd, envp);
		i = 3;
		while (i < ac - 2)
		{
			last_pid = middle_child(av, &oldfd, envp, i);
			i++;
		}
		last_pid = last_child(ac, av, oldfd, envp);
	}
	else
	{
		ft_heredoc(ac, av, &oldfd, envp);
		last_pid = exec_last_cmd_and_append_output(av, oldfd, envp);
	}
	exit_code = wait_processes(last_pid);
	return (exit_code);
}
