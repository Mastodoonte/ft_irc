#ifndef USER_HPP
#define USER_HPP

# include <netinet/in.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <iostream>
# include <string>
# include <ctime>
# include <map>
#include <cstdlib>
#include <stdlib.h>
#include "Channel.hpp"
# define NORMAL       "\033[0m"
# define RED         "\033[31m"            
# define GREEN       "\033[32m"   
# define YELLOW      "\033[33m"
# define BLUE        "\033[34m"

class	User
{
    public:
	    User(void);
	    ~User(void);

	    int acceptUsr(int	server_socket);
	    int	getSocket(void) {return (socket);}
		std::string getNickname(void) {return (nickname);} const
	    sockaddr_in	&getAddr(void){return (addr);}
	    void    clear(void) {close(socket);}
	    void    chooseCMD(char *buffer);
	    void    welcomeNewUser(void);

	    void    commandCAP(std::string &buffer);
	    void    commandNICK(std::string &buffer);
	    void    commandUSER(std::string &buffer);
		void	commandPASS(std::string &buffer);
		void	commandMODE(std::string &buffer);
		void	commandJOIN(std::string &buffer);

		bool    checkIfRegistred(void);
		void	sendClient(const std::string packet);
		void	Registration(std::string packet);

	    User    &operator=(const User &cp);
		bool	caps;
		bool	capsend;
		bool	pass;
		bool	nick;
		bool	user;
		bool	welcome;

	    std::string  username;
	    std::string  nickname;
	    std::string  realname;
	    std::string	 ipaddr;
		std::string	 str_buffer;
		int	socket;
			    
    private:
	    sockaddr_in	addr;
	    socklen_t	socket_len;
	    static std::map<std::string, Channel*>    channels;
	    
};

struct errorReturn : public std::exception
{
	private:

		const char * _strerror;
	public:

		errorReturn(const char *str) : _strerror(str) {};
		const char * what() const throw() { return _strerror; };
};

#endif
