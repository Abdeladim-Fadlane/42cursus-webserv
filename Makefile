NAME = webserv
# curl -i http://127.0.0.1:8082/dir/TOOLS/UTILS/phpinfo.php
# wget 	http://127.0.0.1:8082/dir/TOOLS/UTILS/phpinfo.php
# siege -c 225 -r 1 -b http://127.0.0.1:8082/dir/TOOLS/UTILS/phpinfo.php

CXXFLAGS =  -Wall -Wextra -Werror  -fsanitize=address -g

CXX = c++

CFILES = afadlane/SERVER.cpp \
 		 afadlane/MAIN.cpp \
 		 afadlane/UTILS.cpp \
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
	@   ./webserv 
