/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/13 15:21:32 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

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
		output_fd = open_output(av[ac - 1], append);
		while (i < ac - 2)
			children_process(av[i++], envp, 0);
		dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
		pid.last_pid = children_process(av[ac - 2], envp, 1);
		pid.last_status = wait_processes(pid.last_pid);
		return (pid.last_status);
	}
	else
    {
        ft_putstr_fd("Input Error!\n", 2);
        ft_putstr_fd("Use: ./pipex infile cmd1 cmd2 outfile\n", 2);
        return (EXIT_FAILURE);
    }
	return (EXIT_SUCCESS);
}
