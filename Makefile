NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3

CXXFILES = main.cpp helpers.cpp get_extension.cpp for_header.cpp for_body.cpp \

OBJ = ${CXXFILES:.cpp=.o}

all: ${NAME}

${NAME}: ${OBJ}
	${CXX} ${CXXFLAGS} ${OBJ} -o ${NAME}

clean:
	rm -f ${OBJ} outfile*

fclean: clean
	rm -f ${NAME}

re: fclean all