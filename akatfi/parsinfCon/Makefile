NAME = ExeCo

OBJ = main.o Location.o ConfigFile.o Server.o 

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

run : $(NAME)

$(NAME):$(OBJ)
		c++ $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
		rm -rf $(NAME)

update: fclean run