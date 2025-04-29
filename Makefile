# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: artberna <artberna@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/14 14:31:28 by dsindres          #+#    #+#              #
#    Updated: 2025/04/24 14:47:21 by artberna         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME     := ircserv

CXX      := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -Iinclude

SRC_DIR  := src
OBJ_DIR  := obj

SRC      := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/Client.cpp \
	$(SRC_DIR)/Command.cpp \
	$(SRC_DIR)/Channel.cpp \

OBJ      := \
	$(OBJ_DIR)/main.o \
	$(OBJ_DIR)/Server.o \
	$(OBJ_DIR)/Client.o \
	$(OBJ_DIR)/Command.o \
	$(OBJ_DIR)/Channel.o \

# Rules

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "✅ Compilation terminée !"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "🧹 Objets supprimés."

fclean: clean
	@rm -f $(NAME)
	@echo "🧹 Exécutable supprimé."

re: fclean all

.PHONY: all clean fclean re