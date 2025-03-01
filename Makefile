# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/27 20:47:37 by erosas-c          #+#    #+#              #
#    Updated: 2025/03/01 19:55:02 by ccarrace         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME 		= ircserv

CC 			= c++
RM 			= rm -f
FLAGS		= -Wall -Wextra -Werror -std=c++98 -pedantic

SRC 		= 	main.cpp Server.cpp Client.cpp Channel.cpp join.cpp cap.cpp pass.cpp \
				nick.cpp user.cpp ping.cpp pong.cpp privmsg.cpp utils.cpp
				
OBJ 		= $(SRC:.cpp=.o)
DEP			= $(SRC:.cpp=.d)

all: $(NAME)

$(NAME): $(OBJ) Makefile
	$(CC) $(FLAGS) $(OBJ) -o $@

%.o: %.cpp
	$(CC) $(FLAGS) -MMD -MP -c $< -o $@

-include $(DEP)

clean:
	$(RM) $(OBJ) $(DEP)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re