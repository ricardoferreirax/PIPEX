/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 14:01:37 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/08/10 22:53:28 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex_bonus.h"

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

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			idx;
	unsigned char	*tmp_s1;
	unsigned char	*tmp_s2;

	if (n == 0)
		return (0);
	tmp_s1 = (unsigned char *)s1;
	tmp_s2 = (unsigned char *)s2;
	idx = 0;
	while (tmp_s1[idx] && tmp_s2[idx] && tmp_s1[idx] == tmp_s2[idx]
		&& idx < n - 1)
		idx++;
	return (tmp_s1[idx] - tmp_s2[idx]);
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

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	idx;

	if (size == 0)
		return (ft_strlen(src));
	idx = 0;
	while (src[idx] != '\0' && (idx < size - 1))
	{
		dest[idx] = src[idx];
		idx++;
	}
	dest[idx] = '\0';
	return (ft_strlen(src));
}
