#ifndef RPL_HPP
#define RPL_HPP
#include <errno.h>
# include <unistd.h>
#include <string.h>
# include <iostream>
#include "user.hpp"

std::string	RPL_YOURHOST(const User* client);
std::string	RPL_WELCOME(const User* client, const std::string& network);
std::string	ERR_NEEDMOREPARAMS(const User* client, const std::string packet);
void	PING(const User* client,std::string packet);

#endif