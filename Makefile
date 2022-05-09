ESC_SEQ = \033[
BLUE = $(ESC_SEQ)34m
RED= $(ESC_SEQ)31m
YELLOW = $(ESC_SEQ)33m
GREEN = $(ESC_SEQ)32m
BOLD = $(ESC_SEQ)1m
MOVE_UP = $(ESC_SEQ)1A
ERASE = \r$(ESC_SEQ)K
ERASE_ALL = $(ESC_SEQ)M
ESC_STOP = $(ESC_SEQ)0m
PRINT_INTERLINE = printf '$(GREEN)$(BOLD)================================================================================$(ESC_STOP)\n'
PRINT_INTERLINE_N = printf '$(YELLOW)$(BOLD)================================================================================$(ESC_STOP)\n'


NAME_S	=	server
NAME_C = 	client

SRCS	:=	server.cpp \
		user.cpp \

HEAD	:=	include/server.hpp \
		include/user.hpp \
			

CLIENT_SRC = client.cpp

CLIENT_HEADER = include/client.hpp

C++		= c++ -Wall -Wextra -Werror -std=c++98

OBJS	=	$(SRCS:.cpp=.o)

CLIENT_OBJS = $(CLIENT_SRC:.cpp=.o)

all:		$(NAME_S)
	@printf '$(GREEN)$(BOLD)=====================================================================\n';
	@printf '$(YELLOW)$(BOLD)|          ____  _____         _     ___   ____                     |\n';
	@printf '$(YELLOW)$(BOLD)|          |__     |           |    |__/   |                        |\n';
	@printf '$(YELLOW)$(BOLD)|          |       |           |    |  \   |___                     |\n';
	@printf '$(YELLOW)$(BOLD)|                                                                   |\n';
	@printf '$(GREEN)$(BOLD)===================================================================== \n';
	@printf '$(YELLOW)$(BOLD)%s ${NAME_S} $(ESC_STOP)$(YELLOW) built$(ESC_STOP) √\n' '$@'


$(NAME_S):	$(OBJS)
			$(C++) $(OBJS) -o $(NAME_S)



client:		$(CLIENT_OBJS)
			@$(C++) $(CLIENT_OBJS) -o $(NAME_C)
	@printf '$(YELLOW)$(BOLD)%s ${NAME_C} $(ESC_STOP)$(YELLOW) built$(ESC_STOP) √\n' '$@'
			

clean:
			rm -rf $(OBJS) $(CLIENT_OBJS)

.cpp.o:
			$(C++) -c $< -o $(<:.cpp=.o)

$(OBJS):	$(HEAD)

$(CLIENT_OBJS):	$(CLIENT_HEADER)

fclean:		clean
			rm -rf $(NAME_S) $(NAME_C)

re:			fclean all

.PHONY:		all clean fclean re client
