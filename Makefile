# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/28 14:56:48 by rmedeiro          #+#    #+#              #
#    Updated: 2025/09/04 09:03:23 by rmedeiro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = pipex

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -rf

OBJS        = $(SRCS:.c=.o)

SRCS = \
	srcs/pipex/cmd_path.c \
	srcs/pipex/exec_cmd.c \
	srcs/pipex/pipex.c \
	srcs/pipex/processes.c \
	srcs/utils/ft_split_quotes.c \
	srcs/utils/ft_split.c \
	srcs/utils/pipex_errors.c \
	srcs/utils/pipex_utils_1.c \
	srcs/utils/pipex_utils_2.c

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(NAME) created!"

%.o: %.c
	$(CC) $(CFLAGS) -I includes -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re