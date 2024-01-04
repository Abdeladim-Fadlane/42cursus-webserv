NAME = webserv

SRC = main.cpp\
	Server.cpp\
	Location.cpp\
	Config/Confi_Pars.cpp\
	Config/fun.cpp\
	# ex00/Configuration.cpp\
	# ex00/Server.cpp\
	# ex00/Location.cpp\


CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3


OBJ = ${SRC:.cpp=.o}

all: ${NAME}

${NAME}: ${OBJ}
	$(CXX) ${CXXFLAGS} $(OBJ) -o ${NAME}

clean:
	rm -f ${OBJ}

fclean: clean
	rm -f ${NAME}

re: fclean all