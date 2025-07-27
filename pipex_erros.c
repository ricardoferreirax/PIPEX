/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_erros.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:33:23 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/27 17:33:55 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"



void error_exit(const char *message)
{
    perror(message);
    exit(1);
}

void cmd_notfound(const char *cmd)
{
    ft_putstr_fd("command not found: ", 2);
    ft_putendl_fd(cmd, 2);
    exit(127);
}