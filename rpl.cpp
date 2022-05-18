#include "include/rpl.hpp"

std::string	RPL_YOURHOST(const User* client)
{
    (void)client;
    return (NULL);
}

std::string	RPL_WELCOME(const User* client, const std::string& network)
{
    (void)client;
    (void) network;
        std::string RPL_WELCOME = ":127.0.0.1 001 Welcome to the Internet Relay Network";
//   RPL_WELCOME += nickname + "!" + username + "@127.0.0.1\n";
////    std::string RPL_YOURHOST = "Your host is FT_IRC, running version 1.0\n";
 //   std::string RPL_CREATED = "This server was created <date>\n";
 ////   std::string RPL_MYINFO = "FT_IRC 1.0 0 0\n";

  /*  send_to_client(001,RPL_WELCOME.c_str(), nickname, socket );
    send_to_client(002,RPL_YOURHOST.c_str(), nickname, socket );
    send_to_client(003,RPL_CREATED.c_str(), nickname, socket );
    send_to_client(004,RPL_MYINFO.c_str(), nickname, socket );*/
    return (NULL);
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
	if (packet.size() < 2)
		throw errorReturn(strerror(errno));
	std::string line = ("PONG\r\n");
	std::cout << "-> " << packet << std::endl;

	if (send(client->socket, line.c_str(), line.length(), 0) == -1)
		throw errorReturn(strerror(errno));
}

