/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 15:14:38 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/13 15:18:20 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

int open_input(char *file)
{
    int fd;
    fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDIN_FILENO) == -1)
    {        
        perror("Error duplicating file descriptor");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    return (fd);
}

int open_output(char *file, int append)
{
    int fd;

    if (append)
        fd = open(file, O_CREAT | O_APPEND | O_WRONLY, 0644);
    else
        fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd == -1)
    {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    return (fd);
}