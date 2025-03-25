# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/27 20:47:37 by erosas-c          #+#    #+#              #
#    Updated: 2025/03/25 20:39:08 by erosas-c         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME 		= ircserv

CC 			= c++
RM 			= rm -rf
FLAGS		= -Wall -Wextra -Werror -std=c++98 -pedantic -fsanitize=address

# --- Folders ---------------------------------------------------------------- #

SRC_DIR		= src/
INC_DIR		= inc/
OBJ_DIR		= obj/

# --- Files ------------------------------------------------------------------ #

SRC_FILES	=	main.cpp Server.cpp Client.cpp Channel.cpp utils.cpp \
				cap.cpp debug.cpp invite.cpp join.cpp kick.cpp mode.cpp nick.cpp \
				part.cpp pass.cpp ping.cpp pong.cpp privmsg.cpp quit.cpp topic.cpp \
				user.cpp
				
OBJ_FILES	= $(SRC_FILES:.cpp=.o)
DEP_FILES	= $(OBJ_FILES:.o=.d)

# --- Full Paths ------------------------------------------------------------- #

SRC			= $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ			= $(addprefix $(OBJ_DIR), $(OBJ_FILES))
DEP         = $(addprefix $(OBJ_DIR), $(DEP_FILES))

# --- Compiling rules -------------------------------------------------------- #

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ_DIR) $(OBJ) Makefile
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
