/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:13:59 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/09 19:22:44 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

pid_t	middle_child(char **av, int *prev_read_fd, char **envp, int i);
pid_t	last_child(int ac, char **av, int prev_read_fd, char **envp);
pid_t   ft_heredoc_pipeline(int ac, char **av, char **envp);
void	first_child(char **av, int *prev_read_fd, char **envp);
void	get_pipe_and_fork(int pipefd[2], pid_t *pid);
void 	safe_dup2(int oldfd, int newfd);
void	ft_free_str(char **str);
void	ft_exec_cmd(char *cmd, char **envp);
char	*ft_cmd_path(char *cmd, char **envp);
void	cmd_not_found_msg(char *cmd);
void	path_not_found_msg(char *cmd);
void 	show_usage_exit2(void);
void	error_exit(const char *message);
int		wait_processes(int last_pid);

size_t	ft_strlcpy(char *dest, const char *src, size_t size);
size_t	ft_strlen(const char *s);
char	**ft_split(const char *s, char c);
char	**ft_split_quotes(char const *s, char c);
char	*ft_strchr(const char *s, int c);
char	*ft_strjoin(char const *s1, char const *s2);
char	**ft_split(const char *s, char c);
char	*get_next_line(int fd);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strdup(const char *s);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
void	ft_putstr_fd(char *s, int fd);
void	ft_putendl_fd(const char *s, int fd);
#endif