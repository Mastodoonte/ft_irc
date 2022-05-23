#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include <map>
#include "Client.hpp"

class Channel
{
    public:
        Channel(const std::string &src);
        Channel(const std::string &src, const std::string &password);
        Channel(const Channel &src);
        Channel operator=(const Channel &src);
        ~Channel();

	std::string getName(void);
        bool namedCorrectly(void);
        Channel *lookForJoin(std::map<std::string, Channel*> channels_list, std::string chan_name);

        
    private:
        Channel(){};
        std::string             _name;
        std::string             _password;
        bool                    _isClosed;

        std::vector<std::string>     _chan_invited;
        std::vector<std::string>     _chan_operators;
        std::vector<Client*>         _chan_clients;
        std::vector<std::string>     _chan_banned;


};

Channel *createOrJoin(std::map<std::string, Channel*> &channels_list,  std::string chan_name);

#endif
