#ifndef RPL_HPP
# define RPL_HPP
# include <errno.h>
# include <unistd.h>
# include <string.h>
#include <sstream>
# include <iostream>
# include "user.hpp"


std::string	RPL_YOURHOST        (const User *client);
std::string	RPL_WELCOME         (User *client, const std::string &network);
std::string	RPL_YOURHOST        (User *client, const std::string &network);
std::string	RPL_CREATED         (User *client, const std::string &network);
std::string	RPL_MYINFO          (User *client, const std::string &network);
void	    RPL_PING            (const User *client,std::string packet);
std::string	RPL_MODE( User* client, std::string packet) ;
std::string RPL_MOTD1           (User *client, const std::string &network);
std::string RPL_MOTD2           (User *client, const std::string &network);
std::string RPL_MOTD3           (User *client, const std::string &network);
std::string	ERR_NEEDMOREPARAMS  (const User* client, const std::string &packet);
std::string	ERR_UNKNOWNCOMMAND  (const User *client, const std::string packet);
std::string	ERR_UMODEUNKNOWNFLAG(const User *client, const std::string packet);

#endif
