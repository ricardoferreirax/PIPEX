/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/25 21:02:05 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void	ft_free_str(char **str)
{
	int	i;

	i = 0;
	if (str && *str == NULL)
		return ;
	while (str[i])
		free(str[i++]);
	free(str);
    str = NULL;
}

char *ft_env_path(char **envp)
{
    int i;

    i = 0;
    if (!envp)
        return (NULL);
    while (envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
            return (envp[i]);
        i++;
    }
    return (NULL);
}

char *ft_join_and_check(char *path, char *cmd)
{
    char * tmp;
    char *fullpath;

    tmp = ft_strjoin(path, "/");
    if (!tmp)
        return (NULL);
    fullpath = ft_strjoin(tmp, cmd);
    free(tmp);
    if (!fullpath)
        return (NULL);
    if (access(fullpath, X_OK) == 0) // check if the command is executable
        return (fullpath); // return the full path to the command
    free(fullpath);
    return (NULL);
}

char *ft_cmd_path(char *cmd, char** envp)
{
    int i;
    char **paths;
    char *fullpath;
    char *env_path;

    env_path = ft_env_path(envp);
    if (!env_path)
        return (NULL);
    if (access(cmd, X_OK) == 0) // check if the command is executable
        return (ft_strdup(cmd));
    paths = ft_split(env_path, ':'); // split PATH into directories
    i = 0;
    while (paths[i])
    {
        fullpath = ft_join_and_check(paths[i], cmd);
        if (fullpath)
        {
            ft_free_str(paths);
            return (fullpath); // return the full path to the command
        }
        i++;
    }
    ft_free_str(paths);
    return (NULL);
}

void execute_command(char *cmd, char **envp)
{
    char **args;
    char *path;

    args = ft_split(cmd, ' '); // split the command into arguments
    if (!args || !args[0])
    {
        ft_free_str(args);
		ft_printf("invalid command\n");
		exit(1);
	}
    path = cmd_path(...); // find the path of the command
    if (!path)
	{
        ft_free_str(args);
		ft_putstr_fd("command not found: ", 2);
		ft_putendl_fd(args[0], 2);
		exit(127);
	}
    exceve(path, args, envp); // execute the command with the environment variables
    perror("execve failed"); // if execve fails, print error message
    ft_free_str(args); // free the arguments
    free(path);
    exit(1); // exit if execve fails
}

void cmd1_process(char **av, int pipefd[2], char **envp)
{
    int infile;

    infile = open(av[1], O_RDONLY, 0444); // open file1 (av[1]) for reading
    if (infile == -1)
    {
        close (pipefd[1]);
        ft_putstr_fd("error opening file1", 2);
        exit(1);
    }
    dup2(infile, STDIN_FILENO); // redirect input from file1 (infile) -> stdin: infile ()
    dup2(pipefd[1], STDOUT_FILENO); // redirect output to pipe (pipefd[1]) -> stdout: pipefd[1] (write on pipe)
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(infile); // close file descriptor for file1 (close input_fd)
    execute_command(av[2], envp); // execute the command (cmd)
}

void cmd2_process(char **av, int pipefd[2], char **envp)
{
    int outfile;

    outfile = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644); // open file2 (av[4]) for writing
    if (outfile == -1)
    {
        close(pipefd[0]);
        ft_putstr_fd("error opening file2", 2);
        exit(1);
    }
    dup2(pipefd[0], STDIN_FILENO); // redirect input from pipe (pipefd[0]) -> stdin: pipefd[0] (read from pipe)
    dup2(outfile, STDOUT_FILENO); // redirect output to file2 (outfile) -> stdout: outfile (write on file2)
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(outfile); // close file descriptor for file2 (close output_fd)
    execute_command(av[3], envp); // execute the command (cmd)
}

/* void	pipe_process(int p_fd[2], int fd1, int fd2, char **av, char **envp)
{
	pid_t	pid1;
	pid_t	pid2;

	pid1 = fork();
	if (pid1 == -1)
	{
		perror("fork 1 fail");
		exit(1);
	}
	if (pid1 == 0)
	{
		cmd1_process(p_fd, fd1, av[2], envp);
		exit(1);
	}
	pid2 = fork();
	if (pid2 == -1)
	{
		perror("fork 2 fail");
		exit(1);
	}
	if (pid2 == 0)
	{
		cmd2_process(p_fd, fd2, av[3], envp);
		exit(1);
	}
	close(p_fd[0]);
	close(p_fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
} */

#include <sys/types.h>

int main(int ac, char **av, char **envp)
{
    int pipefd[2];
    pid_t pid;

    if (!envp)
    {
        ft_putstr_fd("no environment variablesfound\n", 2);
        exit(1);
    }
    if (ac == 5)
    {
       if (pipe(pipefd) == -1)
        {
            perror("failed to create pipe");
            exit (1);
        }
        pid = fork();
        if (pid == -1)
        {
            perror("fork failed");
            exit (1);
        }
        else if (pid == 0)
            cmd1_process(av, pipefd, envp);
        else
            cmd2_process(av, pipefd, envp);
    }
    else
    {
        ft_putstr_fd("./pipex file1 cmd1 cmd2 file2\n", 2);
        exit(1);
    }
    return (0);
}
