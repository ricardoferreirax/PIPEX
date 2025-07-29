/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 12:25:22 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/29 12:32:37 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	ft_free_str(char **str)
{
	int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
		free(str[i++]);
	free(str);
}

void    error_exit(const char *message)
{
    perror(message);
    exit(1);
}

void    cmd_notfound(char *cmd)
{
    ft_putstr_fd("pipex: ", 2);
    ft_putstr_fd(cmd, 2);
    ft_putendl_fd(": command not found", 2);
    exit(127);
}

void cmd_not_file_dir(char *cmd)
{
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(": No such file or directory", 2);
	exit(127);
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
    char **path_list;
    char *path;
    char *env_path;

    if (!cmd || !envp)
        return (NULL);
    if (access(cmd, X_OK) == 0)
        return (ft_strdup(cmd));
    env_path = ft_env_path(envp);
    if (!env_path)
        return (NULL);
    path_list = ft_split(env_path, ':');
    if (!path_list)
        return (NULL);
    i = 0;
    while (path_list[i])
    {
        path = ft_join_path(path_list[i], cmd);
        if (path && access(path, X_OK) == 0)
            return (ft_free_str(path_list), path);
        free(path);
        i++;
    }
    return (ft_free_str(path_list), NULL);
}

static void check_cmd_access(char *cmd, char **args)
{
    if (access(args[0], X_OK) != 0)
    {   
        ft_free_str(args);
        cmd_not_file_dir(cmd);
    }
}

void ft_exec_cmd(char *cmd, char **envp)
{
    char **cmd_list;
    char *cmd_path;

    if (!cmd || cmd[0] == '\0')
        cmd_notfound(cmd);
    cmd_list = ft_split(cmd, ' ');
    if (!cmd_list || !cmd_list[0])
    {
        ft_free_str(cmd_list);
        cmd_notfound(cmd);
	}
    if (ft_strchr(cmd_list[0], '/'))
		check_cmd_access(cmd, cmd_list);
    cmd_path = ft_cmd_path(cmd_list[0], envp);
    if (!cmd_path)
	{
        cmd_notfound(cmd_list[0]);
		ft_free_str(cmd_list);
	}
    if (execve(cmd_path, cmd_list, envp) == -1)
	{
		ft_free_str(cmd_list);
		free(cmd_path);
		error_exit("execve failed");
	}
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

void wait_processes(pid_t pid1, pid_t pid2)
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
    wait_processes(pid1, pid2);
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
