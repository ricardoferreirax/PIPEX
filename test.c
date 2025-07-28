/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 11:28:25 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/28 11:50:22 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void error_exit(const char *message)
{
    perror(message);
    exit(1);
}

void cmd_notfound(const char *cmd)
{
    ft_putstr_fd("command not found: ", 2);
    ft_putendl_fd(cmd, 2);
    exit(127);
}

static void handle_cmd_with_path(char *cmd, char **args)
{
    if (access(args[0], X_OK) != 0)
    {
        ft_free_str(args);
        cmd_notfound(cmd);
    }
}

void ft_exec_cmd(char *cmd, char **envp)
{
    char **list_cmd;
    char *path;

    if (!cmd || cmd[0] == '\0')
        cmd_notfound(cmd);
    list_cmd = ft_split(cmd, ' ');
    if (!list_cmd || !list_cmd[0])
    {
        ft_free_str(list_cmd);
        cmd_notfound(cmd);
	}
    if (ft_strchr(list_cmd[0], '/'))
		handle_cmd_with_path(list_cmd[0], list_cmd);
    path = ft_cmd_path(list_cmd[0], envp);
    if (!path)
	{
		ft_free_str(list_cmd);
		cmd_notfound(list_cmd[0]);
	}
    execve(path, list_cmd, envp);
    ft_free_str(list_cmd);
    free(path);
    error_exit("execve failed");
}

static char *ft_env_path(char **envp)
{
    int i;

    i = 0;
    while (envp && envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
            return (envp[i] + 5);
        i++;
    }
    return (NULL);
}

static char *ft_join_path(char *path, char *cmd)
{
    char *tmp;
    char *fullpath;

    if (!path || !cmd)
        return (NULL);
    tmp = ft_strjoin(path, "/");
    if (!tmp)
        return (NULL);
    fullpath = ft_strjoin(tmp, cmd);
    free(tmp);
    if (!fullpath)
        return (NULL);
    return (fullpath);
}

char *ft_cmd_path(char *cmd, char** envp)
{
    int i;
    char **list_path;
    char *path;
    char *env_path;

    if (!cmd || !envp)
        return (NULL);
    if (access(cmd, X_OK) == 0)
        return (ft_strdup(cmd));
    env_path = ft_env_path(envp);
    if (!env_path)
        return (NULL);
    list_path = ft_split(env_path, ':');
    if (!list_path)
        return (NULL);
    i = 0;
    while (list_path[i])
    {
        path = ft_join_path(list_path[i], cmd);
        if (path && access(path, X_OK) == 0)
            return (ft_free_str(list_path), path);
        free(path);
        i++;
    }
    return (ft_free_str(list_path), NULL);
}

void handle_child(char **av, int pipefd[2], char **envp)
{
    int infile;

    infile = open(av[1], O_RDONLY); // open file1 (av[1]) for reading
    if (infile == -1)
    {
		close(pipefd[1]);
		error_exit("error opening input file");
    }
    if (dup2(infile, STDIN_FILENO) == -1) // redirect input from file1 (infile) -> stdin: infile | verify if dup2 was successful
    {
        close(infile);
        close(pipefd[1]);
        error_exit("dup2 failed (infile -> STDIN)");
    }
    if (dup2(pipefd[1], STDOUT_FILENO) == -1) // redirect output to pipe (pipefd[1]) -> stdout: pipefd[1] (write on pipe) | verify if dup2 was successful
    {
        close(infile);
        close(pipefd[1]);
        error_exit("dup2 failed (pipefd[1] -> STDOUT)");
    }
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(infile); // close file descriptor for file1 (close infile)
    ft_exec_cmd(av[2], envp); // execute the command (cmd)
}

void handle_parent(char **av, int pipefd[2], char **envp)
{
    int outfile;

    outfile = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644); // open file2 (av[4]) for writing
    if (outfile == -1)
    {
        close(pipefd[0]);
		error_exit("error opening output file");
    }
    if (dup2(pipefd[0], STDIN_FILENO) == -1) // redirect input from pipe (pipefd[0]) -> stdin: pipefd[0] (read from pipe) | verify if dup2 was successful
    {
        close(outfile);
        close(pipefd[0]);
        error_exit("dup2 failed (pipefd[0] -> STDIN)");
    }
    if (dup2(outfile, STDOUT_FILENO) == -1) // redirect output to file2 (outfile) -> stdout: outfile (write on file2) | verify if dup2 was successful
    {
        close(outfile);
        close(pipefd[0]);
        error_exit("dup2 failed (outfile -> STDOUT)");
    }
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(outfile); // close file descriptor for file2 (close output_fd)
    ft_exec_cmd(av[3], envp); // execute the command (cmd)
}

void wait_for_children(pid_t pid1, pid_t pid2)
{
    int	status1;
	int	status2;
	int	exit_status;

    exit_status = 0;
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	if (WIFEXITED(status1))
		exit_status = WEXITSTATUS(status1);
	if (WIFEXITED(status2))
		exit_status = WEXITSTATUS(status2);
	exit(exit_status);
}

static void pipe_process(char **av, int pipefd[2], char **envp)
{
    pid_t pid1;
    pid_t pid2;
    
    if (pipe(pipefd) == -1)
        error_exit("pipe failed");
    pid1 = fork();
    if (pid1 < 0)
        error_exit("fork failed");
    if (pid1 == 0)
       handle_child(av, pipefd, envp);
    pid2 = fork();
    if (pid2 < 0)
        error_exit("fork failed");
    if (pid2 == 0)
        handle_parent(av, pipefd, envp);
    close(pipefd[0]);
    close(pipefd[1]);
    wait_for_children(pid1, pid2);
}

int	main(int ac, char **av, char **envp)
{
	int	pipefd[2];

	if (ac == 5)
		pipe_process(av, pipefd, envp);
	else
	{
		ft_putstr_fd("Invalid input. Try: ./pipex file1 cmd1 cmd2 file2\n", 2);
		exit(1);
	}
	return (0);
}


void	error_and_exit(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}


void	msg_cmd_not_file(char *cmd)
{
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(": no such file or directory.", 2);
	exit(127);
}
