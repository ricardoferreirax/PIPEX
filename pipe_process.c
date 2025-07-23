/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/23 12:37:06 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>


/*
As said before, inside the pipe, everything goes to one of its ends, one end will write and the other will read.
end[1] is the child process, and end[0] the parent process: the child writes, while the parent reads. 
And since for something to be read, it must be written first, so cmd1 will be executed by the child, 
and cmd2 by the parent.

For the child process, we want infile to be our stdin (we want it as input), and end[1] to be our
stdout (we want to write to end[1] the output of cmd1).
In the parent process, we want end[0] to be our stdin (end[0] reads from end[1] the output of cmd1), 
and outfile to be our stdout (we want to write to it the output of cmd2).

dup2() swaps our files with stdin and stdout. dup2() can swap our fds to stdin/stdout.
It will close fd2 and duplicate the value of fd2 to fd1 (it will redirect fd1 to fd2).

*/

void cmd1_process(int pipefd[2], int input_fd, char *cmd, char **envp)
{
    dup2(input_fd, STDIN_FILENO); // redirect input from file1 (input_fd) -> stdin: input_fd ()
    dup2(pipefd[1], STDOUT_FILENO); // redirect output to pipe (pipefd[1]) -> stdout: pipefd[1] (write on pipe)
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // cclose write end of pipe (pipefd[1])
    close(input_fd); // close file descriptor for file1 (close input_fd)
    execute_command(cmd, envp); // execute the command (cmd)
}

/*
child_process(f1, cmd1)

dup2 close stdin, f1 becomes the new stdin
dup2(f1, STDIN_FILENO); // we want f1 to be execve() input
dup2(end[1], STDOUT_FILENO); // we want end[1] to be execve() stdout
close(end[0]) --> always close the end of the pipe we don't use,
                    as long as the pipe is open, the other end will 
                    be waiting for some kind of input and will not
                    be able to finish its process
close(f1)
// execve function for each possible path
exit(1); // exit the child process if execve fails
*/

/*
e.g: shell infile < grep hello |  wc -l > outfile
         ./pipex infile "grep hello" "wc -l" outfile

1) dup2 (infile, STDIN_FILENO) -> redirect the stdin (descriptor 0) to the infile. STDIN (0) -> infile
   Thus grep hello will read the content from infile.

2) dup2 (pipefd[1], STDOUT_FILENO) -> redirect the stdout (descriptor 1) to the pipe's write end (pipefd[1]).
   Replace stdout (1) with pipefd[1] (write end of pipe). STDOUT (1) -> pipefd[1] 
   Thus grep hello will write its output to the pipe.
   Everything that grep hello prints will be sent to the pipe, not to the terminal.

3) close(pipefd[0]) -> close pipefd[0] (read end of pipe), because cmd1 only writes.

4) execute_cmd(cmd, envp) -> execute the command (cmd) with the environment variables (envp).
   Thus grep hello will be executed with its stdin coming from infile and the stdout going to the pipe.

*/

void cmd2_process(int pipefd[2], int output_fd, char *cmd, char **envp)
{
    dup2(pipefd[0], STDIN_FILENO); // redirect input from pipe (pipefd[0]) -> stdin: pipefd[0] (read from pipe)
    dup2(output_fd, STDOUT_FILENO); // redirect output to file2 (output_fd) -> stdout: output_fd (write on file2)
    close(pipefd[0]); // close read end of pipe (pipefd[0])
    close(pipefd[1]); // close write end of pipe (pipefd[1])
    close(output_fd); // close file descriptor for file2 (close output_fd)
    execute_command(cmd, envp); // execute the command (cmd)
}

/*
parent_process(f2, cmd2);

dup2(f2, ...); // f2 is the stdout
dup2(end[0], ...); // end[0] is the stdin
close(end[1])
close(f2);
// execve function for each possible path
exit(1); // exit the parent process if execve fails
*/

/*

1) dup2 (pipefd[0], STDIN_FILENO) -> redirect the stdin (descriptor 0) to the pipe's read end (pipefd[0]).
   Thus wc -l will read the content from the pipe (what grep hello wrote to the pipe).

2) dup2 (output_fd, STDOUT_FILENO) -> redirect the stdout (descriptor 1) to the file2 (output_fd).
   Replace stdout (1) with output_fd (write end of file2). STDOUT (1) -> output_fd
   Thus wc -l will write its output to file2.

3) close(pipefd[0]) -> close pipefd[0] (read end of pipe), because cmd2 only reads.

4) execute_cmd(cmd, envp) -> execute the command (cmd) with the environment variables (envp).
   Thus wc -l will be executed with its stdin coming from the pipe and the stdout going to file2.
   Execute the wc -l  with the redirected stdin and stdout: 
    - reads from the pipe (pipefd[0]) what grep hello wrote to the pipe
    - writes the output to file2 (output_fd).
*/

void pipe_process(int pipe_fd[2], int fd1, int fd2, char **av, char **envp)
{
    pid_t pid1;
    pid_t pid2;

    pid1 = fork(); // create a child process for cmd1
    if (pid1 == -1)
    {
        error . . .
        exit(1);
    }
    if (pid1 == 0)
    {
        cmd1_process(pipe_fd, fd1, av[2], envp); // child process for cmd1
        exit(1);
    }
    pid2 = fork(); // create a child process for cmd2
    if (pid2 == -1)
    {
        error . . .
        exit(1);
    }
    if (pid2 == 0)
    {
        cmd2_process(pipe_fd, fd2, av[3], envp); // child process for cmd2
        exit(1);
    }
    close (pipe_fd[0]); // close read end of pipe in parent process
    close (pipe_fd[1]); // close write end of pipe in parent process
    waitpid(pid1, NULL, 0); // wait for cmd1 to finish
    waitpid(pid2, NULL, 0); // wait for cmd2 to finish
}

/*
The fork() will split our process in two sub-processes: it returns 0 for the child 
process, a non-zero number for the parent process, or a -1 in case of error.
Also... fork() splits the process in two parallel, simultaneous processes, that happen at 
the same time.

The fork() runs two processes (i.e. two commands) in one single program; 

With waitpid() at the very beginning to wait for the child to finish her process.
With the right stdin and stdout, we can execute the command with execve() 
*/

/*
cmd1 ("grep hello"):
  stdin  <- infile
  stdout -> pipefd[1]

cmd2 ("wc -l"):
  stdin  <- pipefd[0]
  stdout -> outfile
--------------------------------------------------------------
         infile.txt
     ╔═════════════════════╗
     ║    hello world      ║
     ║  this is a  infile  ║
     ║     hello again     ║
     ║    no match here    ║
     ╚═════════════════════╝
            ▼
       "grep hello" (cmd1)
            ▼
      ╔═════════════╗
      ║    PIPE     ║
      ║ hello world ║  ← there only pass the text with "hello"
      ║ hello again ║
      ╚═════════════╝
            ▼
         "wc -l" (cmd2)
            ▼
        outfile.txt
         ╔═════════╗
         ║    2    ║
         ╚═════════╝
----------------------------------------------------------------

*/

