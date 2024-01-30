NAME = webserv

# CXXFLAGS =   -fsanitize=address -g3
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
	@${CXX}   ${OBJ} -o ${NAME}

clean :
	@rm -rf ${OBJ}
	
fclean :clean
	@rm -rf ${NAME}

re : fclean all

run : all clean
	rm -rf www/*
	@  ./webserv akatfi/parsinfCon/file.config
