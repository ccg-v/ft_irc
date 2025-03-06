# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/27 20:47:37 by erosas-c          #+#    #+#              #
#    Updated: 2025/03/05 00:08:53 by ccarrace         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME 		= ircserv

CC 			= c++
RM 			= rm -rf
FLAGS		= -Wall -Wextra -Werror -std=c++98 -pedantic

# --- Folders ---------------------------------------------------------------- #

SRC_DIR		= src/
INC_DIR		= inc/
OBJ_DIR		= obj/

# --- Files ------------------------------------------------------------------ #

SRC_FILES	=	main.cpp Server.cpp Client.cpp Channel.cpp utils.cpp \
				cap.cpp join.cpp mode.cpp nick.cpp pass.cpp ping.cpp pong.cpp \
				privmsg.cpp quit.cpp user.cpp
				
# *.cpp // PER PROVAR

OBJ_FILES	= $(SRC_FILES:.cpp=.o)
DEP_FILES	= $(OBJ_FILES:.cpp=.d)

# --- Full Paths ------------------------------------------------------------- #

SRC			= $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ			= $(addprefix $(OBJ_DIR), $(OBJ_FILES))

# --- Compiling rules -------------------------------------------------------- #

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ) Makefile
	$(CC) $(FLAGS) -I $(INC_DIR) $(OBJ) -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	$(CC) $(FLAGS) -I $(INC_DIR) -MMD -MP -c $< -o $@

-include $(DEP)

# --- Cleaning rules --------------------------------------------------------- #

clean:
	$(RM) $(OBJ) $(DEP)
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re