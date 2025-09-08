# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/28 14:56:48 by rmedeiro          #+#    #+#              #
#    Updated: 2025/09/08 19:01:33 by rmedeiro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = pipex
NAME_BONUS = pipex_bonus

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -rf

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

OBJS = $(SRCS:.c=.o)
OBJS_BONUS = $(SRCS_BONUS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "\033[1;36mCOMPILING AND GENERATE THE EXECUTABLE...\033[0m"
	$(CC) $(CFLAGS) $(OBJS) -o $@
	@echo "$@ created!"

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(OBJS_BONUS)
	@echo "\033[1;36mCOMPILING AND GENERATING EXECUTABLE...\033[0m"
	$(CC) $(CFLAGS) $(OBJS_BONUS) -o $@
	@echo "$@ created!"
	@touch bonus

%.o: %.c
	$(CC) $(CFLAGS) -I includes -c $< -o $@

clean:
	$(RM) $(OBJS) $(OBJS_BONUS)
	@touch bonus
	@rm bonus
	@echo "\033[1;32mCLEANING DONE.\033[0m"

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)
	@echo "\033[1;32mALL CLEANED UP.\033[0m" 

re: fclean all
	@echo "\033[1;34mPIPEX REBUILD.\033[0m"

.PHONY: all clean fclean re bonus