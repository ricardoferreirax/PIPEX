/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 00:31:35 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/13 13:34:16 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

typedef struct s_pid
{
	int		last_pid;
	int		last_status;
}	t_pid;

static char **ft_get_env_paths(char **envp)
{
    char **paths;
    char *env_path;
    int i;

    if (!envp)
        return (NULL);
    i = 0;
    paths = NULL;
    env_path = NULL;
    while (envp[i] && !env_path)
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
            env_path = envp[i] + 5;
        else
            i++;
    }
    if (!env_path)
       return (NULL);
    paths = ft_split(env_path, ':');
    if (!paths)
        exit(1);
    return (paths);
}

static char *ft_join_dir_cmd(char *dir, char *cmd)
{
    char *dir_slash;
    char *fullpath;

    if (!dir || !cmd)
        return (NULL);
    dir_slash = ft_strjoin(dir, "/");
    if (!dir_slash)
        return (NULL);
    fullpath = ft_strjoin(dir_slash, cmd);
    free(dir_slash);
    if (!fullpath)
        return (NULL);
    return (fullpath);
}

char *ft_cmd_path(char *cmd, char** envp)
{
    char **paths;
    char *fullpath;
    int i;

    if (!cmd || !envp)
        return (NULL);
    if (strchr(cmd, '/'))
        return (ft_strdup(cmd));
    paths = ft_get_env_paths(envp);
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i] && paths)
    {
        fullpath = ft_join_dir_cmd(paths[i], cmd);
        if (fullpath && access(fullpath, F_OK | X_OK) == 0)
            return (ft_free_str(paths), fullpath);
        free(fullpath);
        i++;
    }
    ft_free_str(paths);
    return (NULL);
}

static char **ft_parse_cmd(char *cmd)
{
    char **cmd_list;

    cmd_list = ft_split_quotes(cmd, ' ');
    if (!cmd_list || !cmd_list[0])
    {
        cmd_not_found_msg(cmd);
        ft_free_str(cmd_list);
        exit(127);
    }
    return (cmd_list);
}

static void check_cmd_access(char *cmd, char **args)
{
    if (access(args[0], F_OK) != 0)
    {
        path_not_found_msg(cmd);
        ft_free_str(args);
        exit(127);
    }
    else if (access(args[0], X_OK) != 0)
    {
        perror("Pipex");
        ft_free_str(args);
        exit(126);
    }
}

void ft_exec_cmd(char *cmd, char **envp)
{
    char **cmd_list;
    char *cmd_path;

    cmd_list = ft_parse_cmd(cmd);
    if (ft_strchr(cmd_list[0], '/'))
		check_cmd_access(cmd, cmd_list);
    cmd_path = ft_cmd_path(cmd_list[0], envp);
    if (!cmd_path)
    {
        cmd_not_found_msg(cmd_list[0]);
        ft_free_str(cmd_list);
        exit(127);
    }
    if (execve(cmd_path, cmd_list, envp) == -1)
	{   
        perror("execve failed");
		ft_free_str(cmd_list);
		free(cmd_path);
		if (errno == EACCES)
            exit(126);
        else if (errno == ENOENT)
            exit(127);
        exit(EXIT_FAILURE);
	}
}


static void dup2_infile(int infile, int pipe_write)
{
    if (dup2(infile, STDIN_FILENO) == -1)
    {
        close(infile);
        close(pipe_write);
        error_exit("Error. Dup2 failed (stdin)");
    }
    if (dup2(pipe_write, STDOUT_FILENO) == -1)
    {
        close(infile);
        close(pipe_write);
        error_exit("Error. Dup2 failed (stdout)");
    }
}

static void dup2_outfile(int outfile, int pipe_read)
{
    if (dup2(pipe_read, STDIN_FILENO) == -1)
    {
        close(outfile);
        close(pipe_read);
        error_exit("Error. Dup2 failed (pipefd[0] -> STDIN)");
    }
    if (dup2(outfile, STDOUT_FILENO) == -1)
    {
        close(outfile);
        close(pipe_read);
        error_exit("Error. Dup2 failed (outfile -> STDOUT)");
    }
}

void handle_first_child(char **av, int pipefd[2], char **envp)
{
    int infile;

    infile = open(av[1], O_RDONLY);
    if (infile == -1)
    {
		close(pipefd[0]);
        close(pipefd[1]);
		error_exit("Error opening input file");
    }
    close(pipefd[0]);
    dup2_infile(infile, pipefd[1]);
    close(pipefd[1]);
    close(infile);
    ft_exec_cmd(av[2], envp);
}

void handle_second_child(char **av, int pipefd[2], char **envp)
{
    int outfile;

    outfile = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfile == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
		error_exit("Error opening output file");
    }
    close(pipefd[1]);
    dup2_outfile(outfile, pipefd[0]);
    close(pipefd[0]);
    close(outfile);
    ft_exec_cmd(av[3], envp);
}

int	wait_processes(int last_pid)
{
	int	pid;
	int	status;
	int	last_status;

	pid = 1;
	last_status = 0;
	while (pid > 0)
	{
		pid = wait(&status);
		if (pid == last_pid && WIFEXITED(status))
			last_status = WEXITSTATUS(status);
	}
	return (last_status);
}


static pid_t pipe_process(char **av, int pipefd[2], char **envp)
{
    pid_t pid1;
    pid_t pid2;

    if (pipe(pipefd) == -1)
        error_exit("Error creating pipe.");
    pid1 = fork();
	if (pid1 < 0)
	{
		perror("pipex: fork failed");
		exit(1);
	}
    if (pid1 == 0)
       handle_first_child(av, pipefd, envp);
    pid2 = fork();
    if (pid2 < 0)
	{
		perror("pipex: fork failed");
		exit(1);
	}
    if (pid2 == 0)
        handle_second_child(av, pipefd, envp);
    close(pipefd[0]);
    close(pipefd[1]);
    return (pid2);
}

int	main(int ac, char **av, char **envp)
{
    int    pipefd[2];
    int    last_pid;
    int    exit_status;
    
	if (ac != 5)
    {
        ft_putstr_fd("Input Error!\n", 2);
        ft_putstr_fd("Use: ./pipex infile cmd1 cmd2 outfile\n", 2);
        return (EXIT_FAILURE);
    }
    if (av[2][0] == '\0' || av[3][0] == '\0')
        error_exit("Error! Command not valid");
    last_pid = pipe_process(av, pipefd, envp);
    exit_status = wait_processes(last_pid);
    return (exit_status);
}