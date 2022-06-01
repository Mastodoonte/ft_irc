#include "include/rpl.hpp"
#include <vector>
#define VERSION "beta version"
#include <ctime>   
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

//////////////////////////////////////////////////////////////////////
/*                               |SEND|                             */
//////////////////////////////////////////////////////////////////////
 void	sendClient(const User *client, const std::string packet)
{
	//std::string output(packet + "\r\n");
	std::cout  << BLUE<< "-> " << packet << NORMAL;

	if (send(client->socket, packet.c_str(), packet.length(), 0) == -1)
		throw errorReturn(strerror(errno));
}
std::vector<std::string>	ft_extraction(std::string src, char set)
{
	std::vector<std::string>	extract;
	std::string					token;
	std::stringstream	stream(src);
	while (getline(stream, token, set))
		extract.push_back(token);
	return (extract);
}

//////////////////////////////////////////////////////////////////////
/*                                |RPL|                             */
//////////////////////////////////////////////////////////////////////
std::string	RPL_WELCOME(User* client, const std::string& network)
{
    (void)network;
    std::vector<std::string> extract = ft_extraction(client->username, ' ');
    std::string RPL_WELCOME = ":";
    //Florian!AURELIEN@127.0.0.1
    RPL_WELCOME += client->nickname;
    RPL_WELCOME += "!";
    RPL_WELCOME += client->username;
    RPL_WELCOME += "@";
    RPL_WELCOME += "127.0.0.1";
    RPL_WELCOME += " 001 ";
    RPL_WELCOME += client->nickname;
    RPL_WELCOME += " :Welcome to the Internet Relay Network, ";
    RPL_WELCOME += client->nickname;
    RPL_WELCOME += "!";
    RPL_WELCOME += client->nickname;
    RPL_WELCOME += "@";
    RPL_WELCOME += "127.0.0.1";
    //RPL_WELCOME += extract[2];
    RPL_WELCOME += "\r\n";
    return (RPL_WELCOME);
}

std::string	RPL_YOURHOST(User* client, const std::string& network)
{
    (void)network;
    std::vector<std::string> extract = ft_extraction(client->username, ' ');
    std::string RPL_YOURHOST = ":";
    RPL_YOURHOST += getPrefix(*client);
    RPL_YOURHOST += " 002 ";
    RPL_YOURHOST += client->nickname;
    RPL_YOURHOST += " :Your host is ";
    RPL_YOURHOST += network;
    RPL_YOURHOST += ", running ";
    RPL_YOURHOST += VERSION;
    RPL_YOURHOST += "\r\n";
    return (RPL_YOURHOST);
}

std::string	RPL_CREATED(User* client, const std::string& network)
{
 //   std::string RPL_YOURHOST = ":serveur_test 003 flmastor :This server was created <today>\r\n";
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    (void)network;

    std::vector<std::string> extract = ft_extraction(client->username, ' ');
    std::string RPL_CREATED = ":";
    RPL_CREATED += getPrefix(*client);
    RPL_CREATED += " 003 ";
    RPL_CREATED += client->nickname;
    RPL_CREATED += " :This server was created the ";
    RPL_CREATED += SSTR(now->tm_year + 1900);
    RPL_CREATED += "-";
    RPL_CREATED += SSTR(now->tm_mon + 1);
    RPL_CREATED += "-";
    RPL_CREATED += SSTR(now->tm_mday);
    RPL_CREATED += "\r\n";
    return (RPL_CREATED);
}

std::string	RPL_MYINFO(User* client, const std::string& network)
{   //                                          <available user modes> <available channel modes>
    (void)network;
    (void)client;
    std::vector<std::string> extract = ft_extraction(client->username, ' ');
    std::string RPL_MYINFO = ":";
    RPL_MYINFO += getPrefix(*client);
    RPL_MYINFO += " 004 ";
    RPL_MYINFO += client->nickname;
    RPL_MYINFO += " ";
    RPL_MYINFO += VERSION;
    //<available user modes> <available channel modes>
    RPL_MYINFO += " aiwro oitklbI ";
    RPL_MYINFO += "\r\n";
    return (RPL_MYINFO);
}

void	RPL_PING(const User* client, std::string packet)
{
    (void)client;

    std::string output;
    output = ":";
    output += packet;
    output += " PONG ";
    output += packet;
    output += " :";
    output += packet;
    output += "\r\n";
	if (packet.size() < 2)
		throw errorReturn(strerror(errno));
	sendClient(client, output);
}

std::string	RPL_MODE( User* client, std::string packet) 
{
    (void)packet;
    std::vector<std::string>::iterator it = client->mode.begin();
    std::string output = ":";

    output.append(getPrefix(*client).append(" 221 ").append(client->getNickname())).append(" ");
    while (it != client->mode.end())
    {
        output += (*it);
        it++;
    }
    output += "\r\n";
    return (output);
}

std::string	RPL_MOTD1(User* client,const std::string &packet) 
{
    (void)client;
    (void)packet;
    std::string RPL_MOTD1 = ":";
    RPL_MOTD1 += getPrefix(*client);
    RPL_MOTD1 += " 375 ";
    RPL_MOTD1 += client->nickname;
    RPL_MOTD1 += " :- "; 
    RPL_MOTD1 += packet;
    RPL_MOTD1 += " Message of the day -\n"; 
    RPL_MOTD1 += "\r\n";
    return (RPL_MOTD1);

}

std::string	RPL_MOTD2(User* client,const std::string &packet) 
{
    (void)client;
    (void)packet;

    std::string RPL_MOTD2 = ":";
    RPL_MOTD2 += getPrefix(*client);
    RPL_MOTD2 += " 372 ";
    RPL_MOTD2 += client->nickname;
    RPL_MOTD2 += " : "; 

    sendClient(client, RPL_MOTD2 + "\n" );
    sendClient(client,RPL_MOTD2 +"            <=======]}======\n" );
    sendClient(client,RPL_MOTD2 +"              --.   /|\n" );
    sendClient(client,RPL_MOTD2 +"             _\"/_.'/\n" );
    sendClient(client,RPL_MOTD2 +"           .'._._,.'\n" );
    sendClient(client,RPL_MOTD2 +"           :/ \\{}/\n" );
    sendClient(client,RPL_MOTD2 +"           (L  /--',----._\n" );
    sendClient(client,RPL_MOTD2 +"              |          \\\n" );
    sendClient(client,RPL_MOTD2 +"             : /-\\ .'-'\\ / |\n" );
    sendClient(client,RPL_MOTD2 +"             \\\\, ||    \\|\n" );
    sendClient(client,RPL_MOTD2 +"              \\/ ||    ||\n" );
    sendClient(client,RPL_MOTD2 +"Serveur made by **Fayel-ha/Mbaxmann/Flmastor**\n" );
    RPL_MOTD2 += "\n Have a good day then and enjoy "; 
    RPL_MOTD2 += "\r\n";
    return (RPL_MOTD2);

}

std::string	RPL_MOTD3(User* client,const std::string &packet) 
{
    (void)client;
    (void)packet;
    std::string RPL_MOTD3 = ":";
    RPL_MOTD3 += getPrefix(*client);
    RPL_MOTD3 += " 376 ";
    RPL_MOTD3 += client->nickname;
    RPL_MOTD3 += " : "; 
    RPL_MOTD3 += " end of MOTD command "; 
    RPL_MOTD3 += "\r\n";
    return (RPL_MOTD3);
}
//////////////////////////////////////////////////////////////////////
/*                             |ERRORS|                             */
//////////////////////////////////////////////////////////////////////

std::string	ERR_NEEDMOREPARAMS(const User* client, const std::string &packet)
{
    (void)client;
    (void)packet;
    std::vector<std::string> extract = ft_extraction(packet, ' ');
    return (": 461 " + client->nickname + " " + extract[0] + ": Not enough parameters\r\n");
}

std::string	ERR_UNKNOWNCOMMAND(const User *client, const std::string packet)
{
	std::vector<std::string> extract = ft_extraction(packet, ' ');
    return (": 421 "  + client->nickname + " " + extract[0] + " : Unknown command for the server\r\n");
}

std::string	ERR_UMODEUNKNOWNFLAG(const User *client, const std::string packet)
{
	std::vector<std::string> extract = ft_extraction(packet, ' ');
    return (": 501 "  + client->nickname + " " + extract[0] + " : Unknown MODE flag for this server\r\n");
}
