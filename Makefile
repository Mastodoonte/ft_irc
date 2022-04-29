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

SRCS_C	= 	client.cpp 
SRCS_S	= 	server.cpp 

 		
OBJS_C = ${SRCS_C:.cpp=.o}
OBJS_S = ${SRCS_S:.cpp=.o}


HEADER	= include/

NAME = client

NAME1 = server

CLANG = c++ 

C++FLAGS =  -Wall -Werror -Wextra -std=c++98

RM = rm -rf



$(NAME): $(OBJ)
	@printf '$(GREEN)$(BOLD)================================================================================\n';
	@printf '$(YELLOW)$(BOLD)|          ____  _____         _     ___   ____                                |\n';
	@printf '$(YELLOW)$(BOLD)|          |__     |         	|    |__/   |                                   |\n';
	@printf '$(YELLOW)$(BOLD)|          |       |           |    |  \   |___                                |\n';
	@printf '$(YELLOW)$(BOLD)|                                                                              |\n';
	@printf '$(GREEN)$(BOLD)================================================================================ \n';
	@$(CLANG) $(C++FLAGS) -I $(HEADER) -o $(NAME_CLIENT) $(OBJ_C)
	@$(CLANG) $(C++FLAGS) -I $(HEADER) -o $(NAME_SERVER) $(OBJ_S)

%.o : %.cpp
	$(CLANG) $(C++FLAGS) -I $(HEADER) -c $< -o $@
	$(CLANG) $(C++FLAGS) -I include -D STD_ ${SRCS} -o $(NAME1)
test:
	@./ft_test >> out1; ./std_test >> out2; 
	@printf '\n';
	@diff -y out1 out2 
#@rm -f out1 out2
#./a >> out1; ./a_stl >> out2; vimdiff out1 out2
	
debug:
	@g++ -g ${SRCS} -Wall -Werror -Wextra -o debog_exec
	@printf '$(YELLOW)$(BOLD) debog_exec built $(ESC_STOP) √\n' '$@';
	@printf '\n'

clean: 
	@printf '$(GREEN)$(BOLD)================================================================================\n';
	@printf '$(YELLOW)$(BOLD)|          ____  _____         _     ___   ____                                |\n';
	@printf '$(YELLOW)$(BOLD)|          |__     |         	|    |__/   |                                   |\n';
	@printf '$(YELLOW)$(BOLD)|          |       |           |    |  \   |___                                |\n';
	@printf '$(YELLOW)$(BOLD)|                                                                              |\n';
	@printf '$(GREEN)$(BOLD)================================================================================ \n';
	@printf '$(ESC_STOP)'
	${RM} ${OBJS}  makdebog_exec debog_exec.dSYM
	@$(PRINT_INTERLINE)
	@printf '$(RED)$(BOLD)                              %s\ CLEANNING                       \n' '$(NAME)'
	@$(PRINT_INTERLINE)
	@printf '$(RED)$(BOLD) %s\ removed\n' '$(OBJS)'
	@$(PRINT_INTERLINE)
	@printf '\n'
	@printf '\n'
	@rm -f out1 out2

fclean: clean
	@${RM} ${NAME} ${NAME1}
	@$(PRINT_INTERLINE)
	@printf '$(RED)$(BOLD)                              %s\ FCLEAN                          \n' '$(NAME)'
	@$(PRINT_INTERLINE)
	@printf '$(RED)$(BOLD) %s\ removed\n' '$(NAME)' '$(NAME1)'
	@$(PRINT_INTERLINE)

re: fclean all