NAME = webserv


CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3
CXX = c++
CFILES = main.cpp

OBJ = ${CFILES:.cpp=.o}

all: ${NAME}

${NAME} : ${OBJ}
	${CXX} ${CXXFLAGS} -o $@ $^

%.o:%.cpp webserv.hpp
	${CXX} ${CXXFLAGS} -c $<

clean :
	rm-rf ${OBJ}
fclean :clean
	rm-rf ${NAME}

re : fclean all

