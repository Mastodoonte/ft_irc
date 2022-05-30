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


NAME	=	ircserv

C++		= c++ -g -Wall -Wextra -Werror -std=c++98

SRCS	=	main.cpp \
			server.cpp \
			user.cpp \
			Channel.cpp \
			rpl.cpp \
			utils.cpp \

HEAD	=	include/server.hpp \
			include/user.hpp \
			include/rpl.hpp \
			Channel.hpp \
			


OBJS	=	$(SRCS:.cpp=.o)


all:		$(NAME)

$(NAME):	$(OBJS)
	@printf '$(GREEN)$(BOLD)=====================================================================\n';
	@printf '$(YELLOW)$(BOLD)|          ____  _____         _     ___   ____                     |\n';
	@printf '$(YELLOW)$(BOLD)|          |__     |           |    |__/   |                        |\n';
	@printf '$(YELLOW)$(BOLD)|          |       |           |    |  \   |___                     |\n';
	@printf '$(YELLOW)$(BOLD)|                                                                   |\n';
	@printf '$(GREEN)$(BOLD)===================================================================== \n';
	@printf '$(YELLOW)$(BOLD)%s $(ESC_STOP)$(YELLOW) built$(ESC_STOP) √\n' '$@'
	$(C++) $(OBJS) -o $(NAME)

clean:
			rm -rf $(OBJS)

.cpp.o:
			$(C++) -c $< -o $(<:.cpp=.o)

fclean:		clean
			rm -rf $(NAME) $(NAME_C)

re:			fclean all

.PHONY:		all clean fclean re client
