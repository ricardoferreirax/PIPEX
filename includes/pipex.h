/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 13:55:56 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/06 17:54:18 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <errno.h>

void	ft_free_str(char **str);
char	*ft_cmd_path(char *cmd, char **envp);
void    ft_exec_cmd(char *cmd, char **envp);
int     handle_second_child(int pipefd[2], char *file, char *command, char **envp);
int     handle_first_child(int pipefd[2], char *file, char *command, char **envp);
void	wait_children(pid_t pid1, pid_t pid2);
char	*ft_strjoin(char const *s1, char const *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
size_t	ft_strlcpy(char *dest, const char *src, size_t size);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strdup(const char *s);
void	ft_putstr_fd(char *s, int fd);
size_t	ft_strlen(const char *s);
char	**ft_split(const char *s, char c);
char	**ft_split_quotes(char const *s, char c);
void	ft_putendl_fd(const char *s, int fd);
void	ft_putstr_fd(char *s, int fd);
char	*ft_strchr(const char *s, int c);
void    error_exit(const char *message);
void	cmd_not_found_msg(char *cmd);
void	path_not_found_msg(char *cmd);

#endif