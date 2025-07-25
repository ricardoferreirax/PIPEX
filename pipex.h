/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 13:55:56 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/26 15:09:31 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
#include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>

void	ft_free_str(char **str);
char	*ft_cmd_path(char *cmd, char **envp);
void	ft_execute_command(char *cmd, char **envp);
void	handle_child1(char **av, int pipefd[2], char **envp);
void	handle_child2(char **av, int pipefd[2], char **envp);
void	wait_for_children(pid_t pid1, pid_t pid2);
char	*ft_strjoin(char const *s1, char const *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
size_t	ft_strlcpy(char *dest, const char *src, size_t size);
char	*ft_strdup(const char *s);
void	ft_putstr_fd(char *s, int fd);
size_t	ft_strlen(const char *s);
char	**ft_split(const char *s, char c);
void	ft_putendl_fd(char *s, int fd);
void	ft_putstr_fd(char *s, int fd);
void    error_exit(const char *message);

#endif