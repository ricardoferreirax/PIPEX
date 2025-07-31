/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:14:18 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/07/31 12:25:43 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

static int	count_words(const char *s, char sep)
{
	int	idx;
	int	count;
	int	in_word;

	idx = 0;
	count = 0;
	in_word = 0;
	while (s[idx] != '\0')
	{
		if (s[idx] != sep && in_word == 0)
		{
			count++;
			in_word = 1;
		}
		if (s[idx] == sep)
			in_word = 0;
		idx++;
	}
	return (count);
}

static char	*malloc_words(const char **s, char sep)
{
	int			len;
	char		*word;
	const char	*str;

	len = 0;
	while (**s && **s == sep)
		(*s)++;
	str = (*s);
	while (**s && **s != sep)
	{
		len++;
		(*s)++;
	}
	word = malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	ft_strlcpy(word, str, len + 1);
	return (word);
}

static void	free_split(char **array, int current)
{
	while (current >= 0)
		free(array[current--]);
	free(array);
}

char	**ft_split(char const *s, char c)
{
	int		words;
	int		idx;
	char	**strings;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	strings = malloc((words + 1) * sizeof(char *));
	if (!strings)
		return (NULL);
	idx = 0;
	while (idx < words)
	{
		strings[idx] = malloc_words(&s, c);
		if (!strings[idx])
			return (free_split(strings, idx - 1), NULL);
		idx++;
	}
	strings[idx] = NULL;
	return (strings);
}
