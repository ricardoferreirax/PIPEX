# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/28 14:56:48 by rmedeiro          #+#    #+#              #
#    Updated: 2025/09/08 22:12:24 by rmedeiro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = pipex
NAME_B = pipex_bonus

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -rf

HEADER = includes/pipex.h 
HEADER_B = includes/pipex_bonus.h

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

SRCS_B = \
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

RED = \033[1;30;41m
GREEN = \033[1;30;42m
OFF := \033[0m

OBJS = $(SRCS:.c=.o)
OBJS_B = $(SRCS_B:.c=.o)

all: $(NAME) $(NAME_B)

$(NAME): $(OBJS) $(HEADER)
	$(CC) $(CFLAGS) $(OBJS) -o $@
	@echo "$(GREEN) $@ Created $(OFF)"

bonus: $(NAME_B)

$(NAME_B): $(OBJS_B)
	$(CC) $(CFLAGS) $(OBJS_B) -o $@
	@echo "$(GREEN) $@ Created $(OFF)"

%.o: %.c
	$(CC) $(CFLAGS) -I includes -c $< -o $@

clean:
	$(RM) $(OBJS) $(OBJS_B)
	@echo "$(RED) Obj Files Cleaned! $(OFF)"

fclean: clean
	$(RM) $(NAME) $(NAME_B)
	@echo "$(RED) Deleted!  $(OFF)"

re: fclean all

.PHONY: all clean fclean re bonus