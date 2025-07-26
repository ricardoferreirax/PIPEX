/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:01:37 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/26 14:06:03 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

size_t	ft_strlen(const char *s)
{
    size_t	len;

    if (!s)
        return (0);
    len = 0;
    while (s[len] != '\0')
        len++;
    return (len);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*joined;
	size_t	idx;
	size_t	idx2;

	if (!s1 || !s2)
		return (NULL);
	joined = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!joined)
		return (NULL);
	idx = 0;
	while (s1[idx])
	{
		joined[idx] = s1[idx];
		idx++;
	}
	idx2 = 0;
	while (s2[idx2])
	{
		joined[idx + idx2] = s2[idx2];
		idx2++;
	}
	joined[idx + idx2] = '\0';
	return (joined);
}

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	idx;
	size_t	start;

	if (*little == '\0')
		return ((char *)big);
	start = 0;
	while (start < len && big[start] != '\0')
	{
		idx = 0;
		while (little[idx] && big[start + idx] == little[idx]
			&& (start + idx) < len)
			idx++;
		if (little[idx] == '\0')
			return ((char *)&big[start]);
		start++;
	}
	return (NULL);
}

char	*ft_strdup(const char *s)
{
	char	*dest;
	int		idx;

	idx = 0;
	dest = malloc((ft_strlen(s) + 1) * sizeof(char));
	if (!dest)
		return (NULL);
	while (s[idx] != '\0')
	{
		dest[idx] = s[idx];
		idx++;
	}
	dest[idx] = '\0';
	return (dest);
}

void	ft_putstr_fd(char *s, int fd)
{
	int	idx;

	if (!s || fd < 0)
		return ;
	idx = 0;
	while (s[idx] != '\0')
		write(fd, &s[idx++], 1);
}

void	ft_putendl_fd(char *s, int fd)
{
	int	idx;

	idx = 0;
	if (!s || fd < 0)
		return ;
	while (s[idx] != '\0')
		write(fd, &s[idx++], 1);
	write(fd, "\n", 1);
}
