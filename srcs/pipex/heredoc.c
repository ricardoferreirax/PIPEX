/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 00:33:33 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/08 02:33:31 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static void read_till_limiter_and_pipe(char **av, int pipefd[2])
{
    char *line;

    close(pipefd[0]);
    write (STDOUT_FILENO, "heredoc> ", 9);
    while (1)
    {
        line = get_next_line(STDIN_FILENO);
        if (!line)
            break ;
        if (strncmp(line, av[2], ft_strlen(av[2])) == 0 
            && line[ft_strlen(av[2])] == '\n')
        {
            free(line);
            break ;
        }
        write(pipefd[1], line, ft_strlen(line));
        free(line);
        write(STDOUT_FILENO, "heredoc> ", 9);
    }
    close(pipefd[1]);
    exit(0);
}

void    ft_heredoc(int ac, char **av, int *oldfd, char **envp)
{
    int pipefd[2];
    pid_t pid;

    if (ac != 6)
        show_usage_heredoc();
    if (pipe(pipefd) == -1)
        error_exit("Pipe");
    pid = fork();
    if (pid < 0)
        error_exit("Fork Failed!");
    if (pid == 0)
        read_till_limiter_and_pipe(av, pipefd);
    close(pipefd[1]);
    *oldfd = pipefd[0];
    wait(NULL);
    middle_child(av, oldfd, envp, 3);
}

