/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/25 22:04:49 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

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

char *ft_env_path(char **envp)
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
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        fullpath = ft_join_and_check(paths[i], cmd);
        if (fullpath)
            return (ft_free_str(paths), fullpath);
        i++;
    }
    ft_free_str(paths);
    return (NULL);
}

void ft_execute_command(char *cmd, char **envp)
{
    char **args;
    char *path;

    args = ft_split(cmd, ' '); // split the command into arguments
    if (!args || !args[0])
    {
        ft_free_str(args);
		ft_putstr_fd("invalid command: ", 2);
		exit(1);
	}
    path = ft_cmd_path(args[0], envp); // get the full path of the command
    if (!path)
	{
        ft_putstr_fd("command not found: ", 2);
		ft_putendl_fd(args[0], 2);
		ft_free_str(args);
		exit(127);
	}
    execve(path, args, envp);
    perror("execve failed");
    ft_free_str(args);
    free(path);
    exit(1);
}

void cmd1_process(char **av, int pipefd[2], char **envp)
{
    int infile;

    infile = open(av[1], O_RDONLY, 0444); // open file1 (av[1]) for reading
    if (infile == -1)
    {
        perror("error opening input file");
		close(pipefd[1]);
		exit(1);
    }
    dup2(infile, STDIN_FILENO); // redirect input from file1 (infile) -> stdin: infile ()
    dup2(pipefd[1], STDOUT_FILENO); // redirect output to pipe (pipefd[1]) -> stdout: pipefd[1] (write on pipe)
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(infile); // close file descriptor for file1 (close input_fd)
    ft_execute_command(av[2], envp); // execute the command (cmd)
}

void cmd2_process(char **av, int pipefd[2], char **envp)
{
    int outfile;

    outfile = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644); // open file2 (av[4]) for writing
    if (outfile == -1)
    {
        perror("error opening output file");
		close(pipefd[0]);
		exit(1);
    }
    dup2(pipefd[0], STDIN_FILENO); // redirect input from pipe (pipefd[0]) -> stdin: pipefd[0] (read from pipe)
    dup2(outfile, STDOUT_FILENO); // redirect output to file2 (outfile) -> stdout: outfile (write on file2)
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(outfile); // close file descriptor for file2 (close output_fd)
    ft_execute_command(av[3], envp); // execute the command (cmd)
}

void ft_wait_cmd(pid_t pid1, pid_t pid2)
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

void pipe_process(char **av, int pipefd[2], char **envp)
{
    pid_t pid1;
    pid_t pid2;
    
    if (pipe(pipefd) == -1)
    {
        perror("pipe failed");
        exit(1);
    }
    pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork failed");
        exit(1);
    }
    if (pid1 == 0)
       cmd1_process(av, pipefd, envp);
    pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork failed");
        exit(1);
    }
    if (pid2 == 0)
        cmd2_process(av, pipefd, envp);
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    ft_wait_cmd(pid1, pid2);
}

#include <sys/types.h>

int	main(int ac, char **av, char **envp)
{
	int	pipefd[2];

	if (ac == 5)
		pipe_process(av, pipefd, envp);
	else
	{
		ft_putstr_fd("usage: ./pipex file1 cmd1 cmd2 file2\n", 2);
		exit(1);
	}
	return (0);
}
