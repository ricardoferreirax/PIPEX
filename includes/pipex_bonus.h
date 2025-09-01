/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 22:47:41 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/01 16:14:08 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <errno.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

typedef struct s_pid
{
	int		last_pid;
	int		last_status;
}	t_pid;

void	ft_free_str(char **str);
size_t	ft_strlen_g(const char *str);
pid_t	first_child(char *cmd, char **envp);
pid_t    middle_child(char *cmd, char **envp);
pid_t    last_child(char *cmd, char **envp);
int     open_output(char *file, int append);
int     open_input(char *file);
int	    here_doc(char *limiter);
char	*ft_cmd_path(char *cmd, char **envp);
void    ft_exec_cmd(char *cmd, char **envp);
void    handle_first_child(char **av, int pipefd[2], char **envp);
void    handle_second_child(char **av, int pipefd[2], char **envp);
int	    wait_processes(int last_pid);
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
char	*get_next_line(int fd);
char	*join_till_nl(char *line, char *buffer);
int	    ft_strcmp(const char *s1, const char *s2);

#endif