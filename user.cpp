#include "include/user.hpp"
#include "include/rpl.hpp"
#include "include/server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <errno.h>
#define SERVER_NAME "TEST_SERVEUR"
#define VERSION "beta version"

std::map<std::string, Channel*>	    User::channels;
std::vector<std::string>	    User::allNickname;

User::User(void) : caps(false), capsend(false), pass(false), nick(false), user(false), welcome(false), welcome_done(false), change_mode(true), socket(-1) {}
User::~User(void) {
}

User	&User::operator=(const User &cp) {
    if (this == &cp)
	return (*this);
    *this = cp;
    return (*this);
}

int	User::acceptUsr(int server_socket)
{
    char buffer[INET_ADDRSTRLEN];

    socket_len = sizeof(addr);
    socket = accept(server_socket, (struct sockaddr *)&addr, &socket_len);
    if (socket == -1)
    {
		std::cout << "Error to accept client" << std::endl;
		return (-1);
    }
    inet_ntop(AF_INET, &addr.sin_addr.s_addr, buffer, INET_ADDRSTRLEN);
    ipaddr = buffer;
    std::cout <<GREEN << "#   Serveur info: Un nouveau client tente de se connecter : " << inet_ntoa(addr.sin_addr) << " Avec pour socket : " << socket << NORMAL << std::endl;
    return (0);
}

std::map<std::string, Channel *> User::getChannels() const
{
    return (this->channels);
}

//Lors de la connection le client envoi 3 commande
//dans un seul buffer, donc dès qu'une commande
//a été éffectuer je la retire du buffer.

void displayCmd(const std::string cmd)
{
	int pos = 0;
	std::string output ;

    if (cmd.find('\n'))
	{
		pos = cmd.find('\n');
	}
    else
		return ;
	output = cmd.substr(0, pos);
	std::cout << RED  << "<-  " << output << NORMAL << std::endl;
}

static void eraseCMD(std::string *buffer) {
    int pos = 0;

    pos = buffer->find('\n');

    if (pos != -1)
	buffer->erase(0, pos + 1);
    else
	buffer->erase(0, buffer->size());
	
}

 void	User::sendClient(const std::string packet)
{
	std::string output(packet + "\r\n");
	std::cout  << BLUE << "-> " << packet << NORMAL;

	if (send(socket, output.c_str(), output.length(), 0) == -1)
		throw errorReturn(strerror(errno));
}

void	User::Registration(std::string packet)
{
	(void)packet;
	if (!str_buffer.compare(0, 6, "CAP LS"))
	{
		std::string output =+ ":";
		output += SERVER_NAME;
		output += " 421 * CAP";
		sendClient( output);
		std::cout << "\n";
		caps = true;
        eraseCMD(&str_buffer);
		return ;
	}
	if(!str_buffer.compare(0, 6, "CAP END"))
	{
		capsend = true;
		return ;
	}

	if (!str_buffer.compare(0, 4, "NICK"))
	{
		commandNICK(str_buffer);
		nick = true;
         eraseCMD(&str_buffer);
		return ;
	}
	else if (!str_buffer.compare(0, 4, "PASS"))
	{
		if (str_buffer.size() < 2)
			throw errorReturn(strerror(errno));
		pass = true;

        eraseCMD(&str_buffer);
		return ;
	}
    
     if (!str_buffer.compare(0, 4, "USER"))
	{
		if (str_buffer.size() < 4)
		{
			throw errorReturn(strerror(errno));
		}
		commandUSER(str_buffer);
		user = true;
        eraseCMD(&str_buffer);
		return ;
	}
	errorReturn(strerror(errno));
}

bool     User::checkIfRegistred(void)
{
    if (caps == false)
        return (false);
    if (pass == false)
        return (false);
    if (nick == false)
		return (false);
	if (user == false)
		return (false);
    return (true);
}

//Je choisi la bonne commande a call
void	User::chooseCMD(char *buffer, std::map<int, User>	user_tab, int j)
{
    str_buffer = std::string(buffer);
    while (str_buffer.size())
	{
		while (str_buffer.size())
    	{
			displayCmd(str_buffer);
    	    if (!str_buffer.compare(0, 4, "PING"))
			{
			    return (RPL_PING(this, SERVER_NAME));
			}
		    if (checkIfRegistred() == false && welcome == false)
		    {
		    	Registration(str_buffer);
				if (caps == false)
					break ;
				if (pass == false)
					break ;
				if (nick == false)
					break ;
				if (user == false)
					break ;
				if (checkIfRegistred() == true)
				{
		    		welcome = true;
		    		sendClient( RPL_WELCOME(this, SERVER_NAME));
					sendClient( RPL_YOURHOST(this, SERVER_NAME));
					sendClient( RPL_CREATED(this, SERVER_NAME));
					sendClient( RPL_MYINFO(this, SERVER_NAME));
					sendClient( RPL_MOTD1(this, SERVER_NAME));
					sendClient( RPL_MOTD2(this, SERVER_NAME));
					sendClient( RPL_MOTD3(this, SERVER_NAME));
    	        	eraseCMD(&str_buffer);
				}
    	    }
			else if (!str_buffer.compare(0, 4, "MODE"))
		    {
		        commandMODE(str_buffer);
		        eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 4, "NICK"))
		    {
		        commandNICK(str_buffer);
		        eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 4, "JOIN"))
		    {
				commandJOIN(str_buffer, user_tab, j);
				eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 4, "USER"))
		    {
			commandUSER(str_buffer);
			eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 7,"PRIVMSG"))
		    {
		    	commandPRIVMSG(str_buffer, user_tab, j);
		    	eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 4, "PART"))
		    {
			commandPART(str_buffer);
			eraseCMD(&str_buffer);
		    }
			else if (!str_buffer.compare(0, 7,"PRIVMSG"))
			{
				commandPRIVMSG(str_buffer, user_tab, j);
				eraseCMD(&str_buffer);
			}
			else if (!str_buffer.compare(0, 4,"motd"))
			{
				commandMOTD(str_buffer);
				eraseCMD(&str_buffer);
			}
		    else
		    {
			std::cout << "Unknown command: " << str_buffer << std::endl;
			eraseCMD(&str_buffer);
		    }
		}
	}

}

//////////////////////////////////////////////
/*                  Commands                */
//////////////////////////////////////////////

std::vector<std::string>	ft_extract(std::string src, char set)
{
	std::vector<std::string>	extract;
	std::string					res;
	std::string					token;
	int 						pos = 0;

    pos = src.find("\r\n");
    if (pos != -1)
	{
		res = src.substr(0, pos);
	}
    else
	{
		throw errorReturn(strerror(errno));
	}
	std::stringstream	stream(res);
	while (getline(stream, token, set))
		extract.push_back(token);
	return (extract);
}

static std::string  getPrefix(User &usr) {
    std::string	ret = usr.nickname;

    std::vector<std::string> ext = ft_extract(usr.username + "\r\n", ' ');
    return (ret + "!" + ext[0] + "@" + usr.ipaddr);
}

void	User::commandNICK(std::string &buffer)
{    
	//:flmastor NICK TEST
	std::string output;
	std::vector<std::string> extract = ft_extract(buffer, ' ');
	std::vector<std::string>::iterator it = allNickname.begin();
	std::string new_name = extract[1];
	std::string err = "";

	output = ":";
	while (it != allNickname.end())
    {
		if (*it == new_name)
		{
	    	err += ": 443 * " + new_name + " :Nickname is already in use";
	    	err += "\r\n";
	    	sendClient(err);
	    	new_name += "_";
	    	it = allNickname.begin();
		    err = "";
		}
		++it;
    } 
	allNickname.push_back(new_name);
	output += nickname;
	output += " NICK ";
	output += new_name;
	output += "\r\n";

	nickname = new_name;
    std::cout << GREEN  << "#   Serveur info: " << "Socket number# " << getSocket() << " set nickname to " << nickname << NORMAL << std::endl;
	sendClient(output);
}

void	User::commandUSER(std::string &buffer)
{
	std::vector<std::string> extract = ft_extract(buffer, ' ');
	std::vector<std::string>::iterator it = extract.begin();
	it++;
	while(it != extract.end())
	{
		username += *it;
		username += " ";
		it++;
	}
    std::cout << GREEN << "#   Serveur info: " << "Socket number# " <<getSocket() << " set Username to " << username << NORMAL << std::endl;
}

void	User::commandPASS(std::string &buffer)
{
    (void)buffer;
}

void	User::commandMODE(std::string &buffer)
{
	std::vector<std::string> extract = ft_extract(buffer, ' ');
    std::vector<std::string>::iterator it = extract.begin();
	it++;
	it++;
	while(it != extract.end())
	{
		this->mode.push_back(*it);
		it++;
	}
	if (change_mode == true)
	{
		sendClient( RPL_MODE(this, SERVER_NAME));
		change_mode = false;
	}
	else
	{
		;
	}
}

void	User::commandMOTD(std::string &buffer)
{
	(void)buffer;
	sendClient( RPL_MOTD1(this, SERVER_NAME));
	sendClient( RPL_MOTD2(this, SERVER_NAME));
	sendClient( RPL_MOTD3(this, SERVER_NAME));
}

///////////////////////////
//Channel related command//
///////////////////////////

static void splitArg(std::vector<std::string> &chan, std::string buffer) 
{
    int pos = 0;

    while (buffer.size())
    {
	pos = buffer.find(',');
	if (pos == -1)
	    pos = buffer.size();
	chan.push_back(buffer.substr(0, pos));
	buffer.erase(0, pos + 1);
    }
}

static void JOINwelcome(User &usr, Channel &chan) {

    //JOIN message
    std::vector<std::string> ext = ft_extract(usr.nickname + "\r\n", ' ');
    std::string JOINmsg = usr.nickname.c_str();
    JOINmsg += "!" + ext[0] + "@" + usr.ipaddr;
    JOINmsg += " JOIN :" + chan.getName() + "\r\n";
    chan.sendAllClient(JOINmsg);

    //RPL_TOPIC
    std::string RPL_TOPIC = ":";
    RPL_TOPIC += getPrefix(usr) + " 332 " + usr.nickname;
    RPL_TOPIC += " " + chan.getName() + " :<topic>\r\n";
    usr.sendClient(RPL_TOPIC);

    //RPL_NAMERPL
    std::string RPL_NAMERPL = ":";
    std::vector<t_client>::iterator it = chan._chan_clients.begin();
    RPL_NAMERPL += getPrefix(usr) + " 353 " + usr.nickname;
    RPL_NAMERPL += " = " + chan.getName() + " :";
    while (it != chan._chan_clients.end())
    {
	RPL_NAMERPL += " " + it->nickname;
	++it;
    }
    RPL_NAMERPL += "\r\n";
    usr.sendClient(RPL_NAMERPL);

    //RPL_ENDOFNAMES
    std::string RPL_ENDOFNAMES = ":";
    RPL_ENDOFNAMES += getPrefix(usr) + " 366 " + usr.nickname;
    RPL_ENDOFNAMES += " " + chan.getName() + " :End of NAMES list\r\n";
    usr.sendClient(RPL_ENDOFNAMES);

}

void	User::commandJOIN(std::string &buffer, std::map<int, User>	user_tab, int j) {
    std::vector<std::string> chan;
    std::vector<std::string> mode;
    std::string	tmp = buffer;
    int pos1 = 0;
    int pos2 = 0;
    int	key = 1;

    pos1 = buffer.find(' ') + 1;
    if ((pos2 = buffer.find(' ', pos1)) == -1)
    {
	pos2 = buffer.size() - 2;
	key = 0;
    }
    splitArg(chan, buffer.substr(pos1, pos2 - pos1));

    if (key)
    {
	pos1 = pos2 + 1;
        pos2 = buffer.size() - 2;
	splitArg(mode, buffer.substr(pos1, pos2 - pos1));
    }
    std::vector<std::string>::iterator it = chan.begin();
    while (it != chan.end())
    {
	createOrJoin(channels, *it, user_tab, j);
	if (!channels.find(*it)->second->namedCorrectly())
	{
	    channels.erase(*it);
	    std::cout << "Error: invalid argument" << std::endl;
	}
	JOINwelcome(*this, *channels[*it]);
	++it;
    }
}

bool User::chanExist(std::string &chan) {
    std::map<std::string, Channel*>::iterator it = channels.begin();

    while (it != channels.end())
    {
	if (it->second->getName() == chan)
	    return (true);
	++it;
    }
    return (false);
}

bool User::inChan(std::string &nickname, std::string &chan) {
    std::vector<t_client>::iterator it = channels[chan]->_chan_clients.begin();

    while (it != channels[chan]->_chan_clients.end())
    {
	if (it->nickname == nickname)
	    return (true);
	++it;
    }
    return (false);
}

void	User::commandPART(std::string &buffer) {
    std::vector<std::string> chan;
    std::string	partMSG = "";
    int	pos1 = 0;
    int	pos2 = 0;

    pos1 = buffer.find(' ') + 1;
    if ((pos2 = buffer.find(' ', pos1)) == -1)
	pos2 = buffer.size() - 2;
    else
	partMSG = buffer.substr(pos2 + 1, buffer.size() - (pos2 + 2));
    splitArg(chan, buffer.substr(pos1, pos2 - pos1));

    std::vector<std::string>::iterator it = chan.begin();
    while (it != chan.end())
    {
	if (chanExist(*it))
	{
	    if (inChan(nickname, *it))
	    {
		std::string PART_RPL = ":";
		PART_RPL += getPrefix(*this) + " PART " + *it;
		if (partMSG.size())
		    PART_RPL += partMSG;
		PART_RPL += "\r\n";
		sendClient(PART_RPL);
	    }
	    else
	    {
		std::string ERR_NOTONCHANNEL = ":";
		ERR_NOTONCHANNEL += getPrefix(*this) + " 442 " + nickname + " " + *it;
		ERR_NOTONCHANNEL += " :You're not on that channel\r\n";
		sendClient(ERR_NOTONCHANNEL);
	    }
	}
	else
	{
	    std::string ERR_NOSUCHCHANNEL = ":";
	    ERR_NOSUCHCHANNEL += getPrefix(*this) + " 403 " + nickname + " " + *it;
	    ERR_NOSUCHCHANNEL += " :No such channel\r\n";
	    sendClient(ERR_NOSUCHCHANNEL);
	}
	++it;
    }
}

//////////////////////////////////
//End of channel related command//
/////////////////////////////////

void	User::commandPRIVMSG(std::string &buffer, std::map<int, User>	user_tab, int j)
{
	if (channels.size() >= 1)
	{
		std::map<std::string, Channel*>::iterator it = user_tab[j].channels.begin();

		for (std::vector<t_client>::iterator it1 = it->second->_chan_clients.begin(); it1 != it->second->_chan_clients.end(); it1++)
		{
			std::string tmp = ":";
			bool in_Chan = inChan(user_tab[j].nickname, it->second->_name);
			int pos = 0;
			pos = buffer.find(' ') + 1;	
			tmp += getPrefix(user_tab[j]);
			tmp += " " + buffer + "\r\n";
			std::cout  << RED << "-> " << tmp << NORMAL;
			if (j != it1->socket && in_Chan == true)
				send(it1->socket, tmp.c_str(), tmp.size(), 0);
		}
	}
	else if (user_tab.size() >= 1)
	{
		int pos = 0 ;
		int pos1 = 0;
		int i = 0;
		std::string tmp = ":";
		std::string user1;

		pos = buffer.find(' ') + 1;
		pos1 = buffer.find(' ', pos);
		user1 = buffer.substr(pos, pos1 - pos);

		tmp += getPrefix(user_tab[j]);
		tmp += " " + buffer + "\r\n";
		for (std::map<int, User>::iterator it_u = user_tab.begin(); it_u != user_tab.end(); it_u++)
		{
			if (user1 == it_u->second.nickname)
			{
				i = it_u->second.socket;
				break;
			}
		}
		if (user1 == user_tab[i].nickname)
			send (user_tab[i].socket, tmp.c_str(), tmp.size(), 0);
	}
}