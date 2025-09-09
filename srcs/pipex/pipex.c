/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/09 19:31:38 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

int main(int ac, char **av, char **envp)
{
	int prev_read_fd;
	int last_pid;
	int exit_code;

	if (ac != 5)
		show_usage_exit1();
	first_child(av, &prev_read_fd, envp);
	last_pid = last_child(ac, av, prev_read_fd, envp);
	exit_code = wait_processes(last_pid);
	return (exit_code);
}
