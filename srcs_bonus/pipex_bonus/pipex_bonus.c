/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/01 16:22:04 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

static int print_usage_exit(void)
{
	ft_putstr_fd("Error!\nUse: ./pipex infile cmd1 cmd2 cmd[...] outfile", 2);
    return (EXIT_FAILURE);
}

int	main(int ac, char **av, char **envp)
{
	int		i;
	int		output_fd;
	int		append;
	t_pid	pid;

	if (ac > 4)
	{
		append = 0;
		if (ft_strcmp(av[1], "here_doc") == 0)
			append = here_doc(av[2]);
		else
			open_input(av[1]);
		i = 2 + append;
		first_child(av[i++], envp);
		while (i < ac - 2)
			middle_child(av[i++], envp);
		output_fd = open_output(av[ac - 1], append);
		safe_dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
		pid.last_pid = last_child(av[ac - 2], envp);
		pid.last_status = wait_processes(pid.last_pid);
		return (pid.last_status);
	}
	else
		print_usage_exit();
	return (EXIT_SUCCESS);
}
