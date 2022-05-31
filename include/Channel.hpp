#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include <cerrno>
#include <map>
#include <cstring>
#include "user.hpp"

class User;

typedef struct  s_client
{
    int socket;
    std::string nickname;
}               t_client;

class Channel
{
    public:
        Channel(const std::string &src);
        Channel(const std::string &src, const std::string &password);
        Channel(const Channel &src);
	Channel(void);
        Channel operator=(const Channel &src);
        ~Channel();

	    std::string             getName(void);
	    std::string		    getPassword(void);
        std::vector<t_client>   getChanClient(void);
	    void		    setPassword(std::string password);

	bool isOperator(std::string &nickname);
	void addOperator(std::string &nickname);
	void removeOperator(std::string &nickname);
	void sendAllClient(std::string packet);
        bool namedCorrectly(void);
        Channel *lookForJoin(std::map<std::string, Channel*> channels_list, std::string chan_name);

	std::string		topic;
        std::vector<t_client>         _chan_clients;
        std::string             _name;
	
    private:
        //std::string             _name; // Publc
        std::string             _password;
        bool                    _isClosed;

        std::vector<std::string>        _chan_invited;
        std::vector<std::string>        _chan_operators;
        //std::vector<t_client>           _chan_clients; // Public
        std::vector<std::string>        _chan_banned;


};

bool createOrJoin(std::map<std::string, Channel> &channels_list,  std::string chan_name, std::map<int, User>	user_tab, int j, std::string password);

#endif
