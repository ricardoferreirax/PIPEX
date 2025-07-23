/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:08:18 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/23 13:01:51 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ./pipex infile cmd1 cmd2 outfile

/*
The general idea: we read from infile, execute cmd1 with infile as input, send 
the output to cmd2, which will write to outfile.
The pipe() sends the output of the first execve() as input to the second execve();

Each command needs a stdin (input) and returns an output (to stdout).

    infile                                             outfile
as stdin for cmd1                                 as stdout for cmd2            
       |                        PIPE                        ↑
       |           |---------------------------|            |
       ↓           |                           |            |
      cmd1   -->  end[1]       ↔             end[0]   -->  cmd2           
                   |                           |
      cmd1         |---------------------------|           end[0]
      output                                               reads end[1]
      is written                                           and sends cmd1
      to end[1]                                            output to cmd2
      (end[1] becomes                                      (end[0] becomes 
      cmd1 stdout)                                         cmd2 stdin)


The pipe() takes an array of two integers such as int end[2], and links them together. 
In a pipe, what is done in end[0] is visible to end[1], and vice versa. 
Plus, pipe() assigns an fd to each end.
Fd are file descriptors, and since files can be read and written to, by getting an fd each, 
the two ends can communicate: end[1] will write to the its own fd, and end[0] will read end[1]’s
fd and write to its own.

Inside the pipe, everything goes to one of its ends, one end will write and the other will read.
end[1] is the child process, and end[0] the parent process: the child writes, while the parent reads. 
And since for something to be read, it must be written first, so cmd1 will be executed by the child, 
and cmd2 by the parent.

Pipe creates fds. We run pipex like this ./pipex infile cmd1 cmd2 outfile , so infile and outfile need 
to become the stdin and stdout of the pipe.

Our fd table right now looks like this:

                           -----------------    
                 0         |     stdin     |  
                           -----------------    
                 1         |     stdout    |    
                           -----------------    
                 2         |     stderr    |  
                           -----------------
                 3         |     infile    |  // open()
                           -----------------
                 4         |     outfile   |  // open()
                           -----------------
                 5         |     end[0]    | 
                           -----------------
                 6         |     end[1]    |  
                           -----------------
*/

int	main(int ac, char **av, char **envp)
{
    int	pipefd[2];
	int fd1;
	int fd2;
    
    if (ac != 5)
    {
		ft_printf("%s file1 cmd1 cmd2 file2\n", av[0]);
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
	// possibly I need to add pipe_process() here (??)
	close(pipefd[0]);  // close the read of pipe
	close(pipefd[1]);  // close the write of pipe
	close(fd1); // close the input file descriptor
	close(fd2); // close the output file descriptor
	return (0);
}