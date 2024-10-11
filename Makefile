# Nombre del ejecutable
NAME = ircserv

# Compilador y banderas
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Archivos fuente y objetos
SRCS = main.cpp server.cpp client.cpp socketUtils.cpp utils.cpp commands.cpp nickCommand.cpp userCommand.cpp passCommand.cpp joinCommand.cpp channel.cpp pingCommand.cpp pongCommand.cpp whoCommand.cpp modeCommand.cpp

OBJS = $(SRCS:.cpp=.o)

# Incluir directorios (si hay encabezados en carpetas específicas)
INCLUDES = -I.

# Regla por defecto: compilar todo
all: $(NAME)

# Regla para compilar el ejecutable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(NAME) $(OBJS)

# Regla para compilar archivos .o desde .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar archivos objeto
clean:
	rm -f $(OBJS)

# Limpiar archivos objeto y el ejecutable
fclean: clean
	rm -f $(NAME)

# Recompilar todo
re: fclean all

# Regla de phony para evitar conflictos con archivos del mismo nombre
.PHONY: all clean fclean re

