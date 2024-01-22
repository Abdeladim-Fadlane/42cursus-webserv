NAME = webserv

CXXFLAGS =  -Wall -Wextra -Werror -fsanitize=address -g3
CXX = c++

CFILES = afadlane/server.cpp \
 		 afadlane/main.cpp \
 		 afadlane/methods.cpp \
 		 afadlane/cgi.cpp \
 		 afadlane/parsingRequest.cpp \
		 akatfi/Requeste.cpp \
		 akatfi/PostMethod.cpp \

OBJ = ${CFILES:.cpp=.o}

all: ${NAME}

${NAME} : ${OBJ}
	@${CXX} ${CXXFLAGS}  ${OBJ} -o ${NAME}

clean :
	@rm -rf ${OBJ}
fclean :clean
	@rm -rf ${NAME}

re : fclean all

run : all clean
	@  ./webserv
