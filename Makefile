NAME = webserv
# curl -X GET http://127.1.1.0:8080/index.html
# CXXFLAGS =  -fsanitize=address -g3 -Wall -Wextra -Werror 
CXX = c++

CFILES = afadlane/SERVER.cpp \
 		 afadlane/MAIN.cpp \
 		 afadlane/GET.cpp \
 		 afadlane/CGI.cpp \
 		 afadlane/DELETE.cpp \
		 akatfi/Requeste.cpp \
		 akatfi/PostMethod.cpp \
		 akatfi/parsinfCon/ConfigFile.cpp \
		 akatfi/parsinfCon/Server.cpp \
		 akatfi/parsinfCon/Location.cpp \

OBJ = ${CFILES:.cpp=.o}

all: ${NAME}

${NAME} : ${OBJ}
	@${CXX}  ${CXXFLAGS} ${OBJ} -o ${NAME}

clean :
	@rm -rf ${OBJ}
	
fclean :clean
	@rm -rf ${NAME}

re : fclean all

run : all clean
	# rm -rf www/*
	@  ./webserv 
