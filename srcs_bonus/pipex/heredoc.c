/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 00:33:33 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/09 19:23:12 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

static void read_till_limiter(char **av, int pipefd[2])
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
            && line[ft_strlen(av[2])] == '\n' 
            && line[ft_strlen(av[2]) + 1] == '\0')
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

static pid_t    ft_heredoc(char **av, int *hdoc_read_fd)
{
    int pipefd[2];
    pid_t pid;

    get_pipe_and_fork(pipefd, &pid);
    if (pid == 0)
        read_till_limiter(av, pipefd);
    close(pipefd[1]);
    *hdoc_read_fd = pipefd[0];
    return (pid);
}

static pid_t exec_last_and_append(int ac, char **av, int input_fd, char **envp)
{
    int append_fd;
    pid_t pid;

    pid = fork();
    if (pid < 0)
        error_exit("Fork Failed!");
    if (pid == 0)
    {
        append_fd = open(av[ac -1], O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (append_fd == -1)
        {
            close(input_fd);
            error_exit("Error on outfile!");
        }
        safe_dup2(input_fd, STDIN_FILENO);
        safe_dup2(append_fd, STDOUT_FILENO);
        close(append_fd);
        close(input_fd);
        ft_exec_cmd(av[ac - 2], envp);
    }
    close(input_fd);
	return (pid);
}

pid_t ft_heredoc_pipeline(int ac, char **av, char **envp)
{
    pid_t last_pid;
    pid_t hdoc_pid;
    int input_fd;
    int i;

    if (ac < 6)
        show_usage_exit2();
    input_fd = -1;
    hdoc_pid = ft_heredoc(av, &input_fd);
    i = 3;
    last_pid = -1;
    if (i < ac - 2)
    {
        last_pid = middle_child(av, &input_fd, envp, i++);
        waitpid(hdoc_pid, NULL, 0);
        while (i < ac - 2)
           last_pid = middle_child(av, &input_fd, envp, i++);
        last_pid = exec_last_and_append(ac, av, input_fd, envp);
    }
    else
        last_pid = exec_last_and_append(ac, av, input_fd, envp);
    return (last_pid);
}
