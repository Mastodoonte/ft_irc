#include "include/rpl.hpp"
//#include "user.cpp"
#include <bits/stdc++.h>

 void	sendClient(const User *client, const std::string packet)
{
	std::string output(packet + "\r\n");
	std::cout  << BLUE<< "-> " << packet << NORMAL;

	if (send(client->socket, output.c_str(), output.length(), 0) == -1)
		throw errorReturn(strerror(errno));
}

std::string	RPL_WELCOME(const User* client, const std::string& network)
{
    std::string RPL_WELCOME = ":serveur_test 001 flmastor :Welcome to the Internet Relay Network, flmastor!flmastor@localhost\r\n";
    (void)network;
    (void)client;
    return (RPL_WELCOME);
}

std::string	RPL_YOURHOST(const User* client, const std::string& network)
{
    std::string RPL_YOURHOST = ":serveur_test 002 flmastor :Your host is Test_serveur, running version beta\r\n";
    (void)network;
    (void)client;
    return (RPL_YOURHOST);
}

std::string	RPL_CREATED(const User* client, const std::string& network)
{
    std::string RPL_YOURHOST = ":serveur_test 003 flmastor :This server was created <today>\r\n";
    (void)network;
    (void)client;
    return (RPL_YOURHOST);
}

std::string	RPL_MYINFO(const User* client, const std::string& network)
{
    std::string RPL_YOURHOST = ":serveur_test 004 flmastor Test_serveur beta aiwro oitklbI\r\n";
    (void)network;
    (void)client;
    return (RPL_YOURHOST);
}

std::string	ERR_NEEDMOREPARAMS(const User* client, const std::string packet)
{
    (void)client;
    (void)packet;
    return "Error";
    return (": 461 " + client->nickname);
}

void	PING(const User* client, std::string packet)
{
    (void)client;
	if (packet.size() < 2)
		throw errorReturn(strerror(errno));
	//sendClient(": PONG");
}

