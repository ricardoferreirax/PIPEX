/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:19:06 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/23 12:08:06 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>

// ./pipex file1 cmd1 cmd2 file2

// Each command needs a stdin (input) and returns an output (to stdout).

/*
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

