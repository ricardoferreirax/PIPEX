# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/28 14:56:48 by rmedeiro          #+#    #+#              #
#    Updated: 2025/09/09 19:08:18 by rmedeiro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = pipex
B_NAME = pipex_bonus

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -rf

INCLUDE = includes

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

B_SRCS = \
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

B_OBJS = $(B_SRCS:.c=.o)
OBJS = $(SRCS:.c=.o)

RED = \033[1;30;41m
GREEN = \033[1;30;42m
OFF := \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "$(GREEN) ./pipex created! $(OFF)"

bonus: $(B_NAME)

$(B_NAME): $(B_OBJS)
	$(CC) $(CFLAGS) $(B_OBJS) -o $(B_NAME)
	@echo "$(GREEN) ./pipex_bonus created! $(OFF)"

%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(B_OBJS)
	@echo "$(RED) Object Files Cleaned! $(OFF)"

fclean: clean
	$(RM) $(NAME)
	$(RM) $(B_NAME)
	@echo "$(GREEN) All Cleaned! $(OFF)" 

re: fclean $(NAME)

reb: fclean $(B_NAME)

.PHONY: all clean fclean re reb bonus