# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/02 12:37:48 by akatfi            #+#    #+#              #
#    Updated: 2024/01/21 14:26:06 by akatfi           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = server

OBJ = Requeste.o PostMethod.o

CXXFLAGS = -Wall -Wextra -Werror 
all : $(NAME)
$(NAME) : $(OBJ)
	c++ $(OBJ) -o $(NAME)

clean :
	rm -rf  $(OBJ)	

fclean : clean
	rm -rf ../../vovvo/*
	rm -rf $(NAME)

run : fclean all
	rm -rf ../../vovvo/*