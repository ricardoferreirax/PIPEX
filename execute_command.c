/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 12:42:15 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/25 15:15:02 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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