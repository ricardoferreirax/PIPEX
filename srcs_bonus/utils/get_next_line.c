/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 15:37:05 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/08 16:27:52 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

static size_t	ft_strlen_g(const char *str)
{
	size_t	len;

	len = 0;
	if (!str)
		return (0);
	while (str[len] != '\0' && str[len] != '\n' )
		len++;
	if (str[len] == '\n')
		len++;
	return (len);
}

static char	*join_till_nl(char *line, char *buffer)
{
	char	*new_line;
	size_t	line_idx;
	size_t	buffer_idx;

	if (!buffer)
		return (NULL);
	new_line = malloc(sizeof(char) * (ft_strlen_g(line) + ft_strlen_g(buffer) + 1));
	if (!new_line)
		return (NULL);
	line_idx = 0;
	while (line && line[line_idx] != '\0')
	{
		new_line[line_idx] = line[line_idx];
		line_idx++;
	}
	buffer_idx = 0;
	while (buffer[buffer_idx] != '\0')
	{
		new_line[line_idx++] = buffer[buffer_idx];
		if (buffer[buffer_idx++] == '\n')
			break ;
	}
	new_line[line_idx] = '\0';
	free(line);
	return (new_line);
}

char	*get_next_line(int fd)
{
	static char	buffer[FOPEN_MAX][BUFFER_SIZE + 1];
	char		*result_line;
	size_t		read_idx;
	size_t		write_idx;
	int			newl;

	if (BUFFER_SIZE <= 0 || fd < 0 || fd >= FOPEN_MAX)
		return (NULL);
	result_line = NULL;
	newl = 0;
	while (!newl && (buffer[fd][0] || (read(fd, buffer[fd], BUFFER_SIZE) > 0)))
	{
		result_line = join_till_nl(result_line, buffer[fd]);
		read_idx = 0;
		write_idx = 0;
		while (buffer[fd][read_idx] != '\0')
		{
			if (newl)
				buffer[fd][write_idx++] = buffer[fd][read_idx];
			if (buffer[fd][read_idx] == '\n')
				newl = 1;
			buffer[fd][read_idx++] = '\0';
		}
	}
	return (result_line);
}