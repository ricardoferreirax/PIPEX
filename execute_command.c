/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 12:42:15 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/23 13:13:40 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
From the MAN, int execve(const char *path, char *const argv[], char *envp[]);

"execve()  executes the program referred to by pathname.  This causes the
program that is currently being run by the calling  process  to  be  re‐
placed with a new program, with newly initialized stack, heap, and (ini‐
tialized and uninitialized) data segments".

# path: the path to our command we can type `which ls` and `which wc` in terminal
        to see the exact path to the commands' binaries
    
# argv[]: the args the command needs, for example `ls -la` we can use ft_split to obtain
          a char ** like this { "ls", "-la", NULL } it must be null terminated.
          
          "argv is an array of pointers to strings passed to the new program as its
          command-line  arguments.   By  convention,  the  first  of these strings
          (i.e., argv[0]) should contain the filename associated with the file be‐
          ing executed.  The argv array must be  terminated  by  a  null  pointer.
          (Thus, in the new program, argv[argc] will be a null pointer)".

# envp: the environmental variable we can simply retrieve it in our main and pass it onto 
        execve, no need to do anything here in envp we'll see a line PATH which contains all possible
        paths to the commands' binaries.

        "envp  is  an  array  of  pointers to strings, conventionally of the form
        key=value, which are passed as the environment of the new program.   The
        envp array must be terminated by a null pointer".

*/

void execute_comand(char *cmd, char **envp)
{
    char **args;
    char *path;

    args = ft_split(cmd, ' '); // split the command into arguments
    if (!args || !args[0])
    {
        // free(args); ??
        // print error message
        exit(1); // exit if no command is provided
    }
    // path = find_path(...); // I need to create a function to find the path of the command
    if (!path)
    {
        // print error message
        ft_putendl_fd(args[0], 2); // it'll print the command that was not found
        free_split(args);
        exit(127); // exit if the command is not found
    }
    execve(path, args, envp); // execute the command with the environment variables
    // error() -> if execve fails, print error message
    free_split(args); // free the arguments
    free(path);
    exit(1); // exit if execve fails
}