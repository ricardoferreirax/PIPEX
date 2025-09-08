# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/28 14:56:48 by rmedeiro          #+#    #+#              #
#    Updated: 2025/09/08 17:04:56 by rmedeiro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = pipex
NAME_BONUS = pipex_bonus

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -rf

OBJS = $(SRCS:.c=.o)
OBJS_BONUS = $(SRCS_BONUS:.c=.o)

SRCS = \
	srcs/pipex/cmd_path.c \
	srcs/pipex/exec_cmd.c \
	srcs/pipex/pipex.c \
	srcs/pipex/processes.c \
	srcs/utils/ft_split_quotes.c \
	srcs/utils/ft_split.c \
	srcs/utils/pipex_errors.c \
	srcs/utils/libft_utils_1.c \
	srcs/utils/libft_utils_2.c \
	srcs/utils/pipex_utils.c \

SRCS_BONUS = \
	srcs_bonus/pipex/cmd_path.c \
	srcs_bonus/pipex/exec_cmd.c \
	srcs_bonus/pipex/heredoc.c \
	srcs_bonus/pipex/pipex_bonus.c \
	srcs_bonus/pipex/processes.c \
	srcs_bonus/utils/ft_split_quotes.c \
	srcs_bonus/utils/ft_split.c \
	srcs_bonus/utils/get_next_line.c \
	srcs_bonus/utils/libft_utils_1.c \
	srcs_bonus/utils/libft_utils_2.c \
	srcs_bonus/utils/pipex_errors.c \
	srcs_bonus/utils/pipex_utils.c \

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(NAME) created!"

bonus: $(OBJS_BONUS)
	$(CC) $(CFLAGS) $(OBJS_BONUS) -o $(NAME_BONUS)
	@touch bonus

%.o: %.c
	$(CC) $(CFLAGS) -I includes -c $< -o $@

clean:
	$(RM) $(OBJS) $(OBJS_BONUS)
	@touch bonus
	@rm bonus

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)

re: fclean all

.PHONY: all clean fclean re