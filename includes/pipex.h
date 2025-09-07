/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 13:55:56 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/07 00:28:11 by rmedeiro         ###   ########.fr       */
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

typedef struct s_pid
{
	int	last_pid;
	int	last_status;
}		t_pid;

void	ft_free_str(char **str);
char	*ft_cmd_path(char *cmd, char **envp);
void	ft_exec_cmd(char *cmd, char **envp);
void	handle_first_child(char **av, int pipefd[2], char **envp);
void	handle_second_child(char **av, int pipefd[2], char **envp);
int		wait_processes(int last_pid);
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
void	error_exit(const char *message);
void	cmd_not_found_msg(char *cmd);
void	path_not_found_msg(char *cmd);

#endif