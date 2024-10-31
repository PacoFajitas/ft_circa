NAME = ircserv

#########
RM = rm -f
CC = c++
CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -std=c++98 
#########

#########
FILES = channel client cscCommand fileTransfer main server socketUtils utils \
		

FILES += inviteCommand joinCommand kickCommand modeCommand nickCommand partCommand \
		 passCommand pingCommand pongCommand privmsgCommand processCommand \
		 topicCommand userCommand whoCommand bot\

SRC = $(addsuffix .cpp, $(FILES))

INC = inc -I inc/commands
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