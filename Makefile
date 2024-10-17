NAME = ircserv

#########
RM = rm -f
CC = c++
CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -std=c++98 
#########

#########
FILES = channel client commands joinCommand main modeCommand nickCommand\
		passCommand pingCommand pongCommand server socketUtils userCommand\
		utils whoCommand partCommand

FILES += #pass nick user join privmsg topic 

SRC = $(addsuffix .cpp, $(FILES))

INC = inc
vpath %.cpp src src/commands
#########

#########
OBJ_DIR = OBJ
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
DEP = $(addsuffix .d, $(basename $(OBJ)))
#########

#########
$(OBJ_DIR)/%.o: %.cpp 
	@mkdir -p $(@D)
	@${CC}  -I $(INC) $(CFLAGS) -MMD -c $< -o $@

all:
	$(MAKE) $(NAME) --no-print-directory

$(NAME):: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "EVERYTHING DONE   "⠀⠀⠀⠀⠀⠀⠀


clean:
	$(RM) $(OBJ) $(DEP)
	$(RM) -r $(OBJ_DIR)
	@echo "OBJECTS REMOVED   "

fclean: clean
	$(RM) $(NAME)
	@echo "EVERYTHING REMOVED   "

re:	fclean all

.PHONY: all clean fclean re

-include $(DEP)