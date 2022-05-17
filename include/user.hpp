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
	    int	getSocket(void);
	    sockaddr_in	&getAddr(void);
	    void    clear(void);
	    void    chooseCMD(char *buffer);
	    void    welcomeNewUser(void);

	    void    commandCAP(std::string &buffer);
	    void    commandNICK(std::string &buffer);
	    void    commandUSER(std::string &buffer);
		void	commandPASS(std::string &buffer);
		void	commandMODE(std::string &buffer);
	    time_t  ping(void);
	    User    &operator=(const User &cp);
    private:
	    int	socket;
	    sockaddr_in	addr;
	    socklen_t	socket_len;
	    
	    std::string  username;
	    std::string  nickname;
	    std::string  realname;
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
