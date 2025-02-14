# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/27 20:47:37 by erosas-c          #+#    #+#              #
#    Updated: 2025/02/14 22:38:58 by ccarrace         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC 		= main.cpp Server.cpp Client.cpp
OBJ 		= $(SRC:.cpp=.o)
CC 			= c++
RM 			= rm -f
FLAGS		= -Wall -Wextra -Werror -std=c++98 -pedantic

NAME 		= ircserv
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $@

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re