/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/08 19:41:38 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

int main(int ac, char **av, char **envp)
{
	int prev_readfd;
	int last_pid;
	int exit_code;
	int i;

	if (ac < 5)
		show_usage_exit2();
	if (ft_strncmp(av[1], "here_doc", 8) != 0)
	{
		first_child(av, &prev_readfd, envp);
		i = 3;
		while (i < ac - 2)
		{
			last_pid = middle_child(av, &prev_readfd, envp, i);
			i++;
		}
		last_pid = last_child(ac, av, prev_readfd, envp);
	}
	else
		last_pid = ft_heredoc_pipeline(ac, av, envp);
	exit_code = wait_processes(last_pid);
	return (exit_code);
}
