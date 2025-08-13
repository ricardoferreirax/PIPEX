# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/28 14:56:48 by rmedeiro          #+#    #+#              #
#    Updated: 2025/08/13 18:25:49 by rmedeiro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = pipex

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -rf

INCLUDE = includes/pipex.h
OBJS = $(SRCS:.c=.o)
OBJS_BONUS = $(SRCS_BONUS:.c=.o)

SRCS = srcs/pipex/cmd_path.c srcs/pipex/exec_cmd.c srcs/pipex/pipex.c srcs/pipex/processes.c \
       srcs/utils/ft_split_quotes.c srcs/utils/ft_split.c srcs/utils/pipex_errors.c \
	   srcs/utils/pipex_utils_1.c srcs/utils/pipex_utils_2.c \

SRCS = srcs_bonus/pipex_bonus/pipex_bonus.c srcs_bonus/pipex_bonus/children_process.c \
       srcs_bonus/pipex_bonus/cmd_path.c srcs_bonus/pipex_bonus/exec_cmd.c \
	   srcs_bonus/pipex_bonus/here_doc.c srcs_bonus/pipex_bonus/open.c \
	   srcs_bonus/utils_bonus/ft_split_quotes.c srcs_bonus/utils_bonus/ft_split.c \
	   srcs_bonus/utils_bonus/get_next_line.c srcs_bonus/utils_bonus/pipex_errors.c \
	   srcs_bonus/utils_bonus/pipex_utils_1.c srcs_bonus/utils_bonus/pipex_utils_2.c
	   
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

bonus: $(BONUS)

$(BONUS): $(OBJS_BONUS)
	$(CC) $(CFLAGS) $(OBJS_BONUS) -o $(BONUS)

%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(OBJS_BONUS)

fclean: clean
	$(RM) $(NAME) $(BONUS)

re: fclean all

test: all
	@chmod +x test_pipex.sh
	@./test_pipex.sh

PHONY: all clean fclean re bonus test