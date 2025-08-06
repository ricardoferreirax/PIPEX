/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:41:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/06 19:09:41 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

int main(int ac, char **av, char **envp)
{
    int pipefd[2];
    int pid1;
    int pid2;

    if (ac != 5)
    {
        ft_putstr_fd("Input Error!\n", 2); 
        ft_putstr_fd("Use: ./pipex infile cmd1 cmd2 outfile\n", 2);
        exit(EXIT_FAILURE);
    }
    if (av[2][0] == '\0' || av[3][0] == '\0')
        error_exit("Error! Command not valid");
    if (pipe(pipefd) == -1)
        error_exit("Error creating pipe");
    pid1 = handle_first_child(pipefd, av[1], av[2], envp);
    pid2 = handle_second_child(pipefd, av[4], av[3], envp);
    close(pipefd[0]);
    close(pipefd[1]);
    wait_children(pid1, pid2);
    return (0);
}
