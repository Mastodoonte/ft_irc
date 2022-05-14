#ifndef USER_HPP
#define USER_HPP

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <ctime>

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

#endif
