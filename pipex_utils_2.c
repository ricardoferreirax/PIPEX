/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:56:56 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/29 16:05:53 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	ft_putstr_fd(char *s, int fd)
{
	int	idx;

	if (!s || fd < 0)
		return ;
	idx = 0;
	while (s[idx] != '\0')
		write(fd, &s[idx++], 1);
}

void	ft_putendl_fd(const char *s, int fd)
{
	int	idx;

	idx = 0;
	if (!s || fd < 0)
		return ;
	while (s[idx] != '\0')
		write(fd, &s[idx++], 1);
	write(fd, "\n", 1);
}

char	*ft_strchr(const char *s, int c)
{
	int		idx;

	idx = 0;
	while (s[idx] != '\0')
	{
		if (s[idx] == (char)c)
			return ((char *)(s + idx));
		idx++;
	}
	if ((char)c == '\0')
		return ((char *)(s + idx));
	return (NULL);
}

void	ft_free_str(char **str)
{
	int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
    {
        free(str[i]);
        str[i] = NULL;
        i++;
    }
	free(str);
}
