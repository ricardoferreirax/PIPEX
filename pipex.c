/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:08:18 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/25 15:13:57 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	main(int argc, char **argv, char **envp)
{
	int	pipefd[2];
	int	fd1;
	int	fd2;

	if (argc != 5)
	{
		ft_printf("%s file1 cmd1 cmd2 file2\n", argv[0]);
		return (1);
	}
	fd1 = open(argv[1], O_RDONLY);
	fd2 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd1 == -1 || fd2 == -1)
	{
		perror("cannot open files");
		return (1);
	}
	if (pipe(pipefd) == -1)
	{
		perror("pipe fail");
		return (1);
	}
	pipe_process(pipefd, fd1, fd2, argv, envp);
	close(pipefd[0]);
	close(pipefd[1]);
	close(fd1);
	close(fd2);
	return (0);
}
