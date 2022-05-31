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
# include <cstdlib>
# include <stdlib.h>
# include "Channel.hpp"
#include "server.hpp"
# define NORMAL       "\033[0m"
# define RED         "\033[31m"            
# define GREEN       "\033[32m"   
# define YELLOW      "\033[33m"
# define BLUE        "\033[34m"

class Channel;

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
	    void    chooseCMD(std::map<int, User>	user_tab, int j, global global);
	    void    welcomeNewUser(void);

	    bool    isBufferReady(std::string to_add);
	    void    clearBuffer(void);
	    void    commandCAP(std::string &buffer);
	    void    commandNICK(std::string &buffer);
	    void    commandUSER(std::string &buffer);
		void	commandPASS(std::string &buffer);
		void	commandMODE(std::string &buffer);
		void	modeUser	(std::string &buffer, int path, std::vector<std::string> extract);
		///////////////////////////
		//Channel related command//
		///////////////////////////
		void	commandJOIN(std::string &buffer, std::map<int, User>	user_tab, int j);
		bool	chanExist(std::string &chan);
		bool	inChan(std::string &nickname, std::string &chan);
		void	commandPART(std::string &buffer);
		void	commandModeChannel(std::string &buffer);
		void	commandTOPIC(std::string &buffer);
		void	commandLIST(std::string &buffer);
		void	commandINVITE(std::string &buffer);
		void	commandKICK(std::string &buffer);
		//////////////////////////////////
		//End of channel related command//
		//////////////////////////////////
		void	commandPRIVMSG(std::string &buffer, std::map<int, User>	user_tab, int j);
		void	chanPRIVMSG(std::string &buffer, std::map<int, User> user_tab, int j);
		void	userPRIVMSG(std::string &buffer, std::map<int, User> user_tab, int j);
		void	commandQUIT(std::string &buffer, std::map<int, User> user_tab, int j);
		void	commandMOTD(std::string &buffer);

		bool    checkIfRegistred(void);
		void	sendClient(const std::string packet);
		void	Registration(std::string packet, global global);

	    User    &operator=(const User &cp);
		bool	caps;
		bool	capsend;
		bool	pass;
		bool	nick;
		bool	user;
		bool	welcome;
		bool	welcome_done;
		bool	change_mode;

	    std::string	 str_buffer;
	    std::string  username;
	    std::string  nickname;
	    std::string  realname;
	    std::string	 ipaddr;
		int	socket;

		std::map<std::string, Channel> getChannels() const;
		static std::map<std::string, Channel>    channels;
		static std::vector<std::string> allNickname;
		std::vector<std::string>	mode;
		std::string getMode(void);
    private:
	    sockaddr_in	addr;
	    socklen_t	socket_len;
	    //static std::map<std::string, Channel*>    channels;
	    
};

std::string getPrefix(User &usr);

struct errorReturn : public std::exception
{
	private:
		const char * _strerror;
	
	public:
		errorReturn(const char *str) : _strerror(str) {};
		const char * what() const throw() { return _strerror; };
};

 struct badPassword: public std::exception
    {
        virtual const char* what() const throw() 
        {
            return ("Bad Password written by client");
        }
    };

#endif
