/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:56:56 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/31 12:38:59 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

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

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char		*substring;
	size_t		idx;

	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	substring = malloc((len + 1) * sizeof(char));
	if (!substring)
		return (NULL);
	idx = 0;
	while (idx < len)
		substring[idx++] = s[start++];
	substring[idx] = '\0';
	return (substring);
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
