/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:08:18 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/23 11:15:54 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	main(int ac, char **av, char **envp)
{
    int	pipefd[2];
	int fd1;
	int fd2;
    
    if (ac != 5)
    {
		ft_putstr("%s file1 cmd1 cmd2 file2\n", av[0]);
		return (1);
    }
	fd1 = open(av[1], O_RDONLY);
	fd2 = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd1 == -1 || fd2 == -1)
	{
		error . . .
		return (1);
	}
	if (pipe(pipefd) == -1)
	{
		error . . .
		return (1);
	}
	// add pipe_process()
	close(pipefd[0]);  // close the read of pipe
	close(pipefd[1]);  // close the write of pipe
	close(fd1);
	close(fd2);
	return (0);
}