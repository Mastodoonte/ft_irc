#include "include/user.hpp"
#include "include/rpl.hpp"
#include "include/server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <errno.h>
#include <sstream>
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()
#define SERVER_NAME "TEST_SERVEUR"
#define VERSION "beta version"

std::map<std::string, Channel>	    User::channels;
std::vector<std::string>	ft_extract(std::string src, char set);
std::vector<std::string>	    User::allNickname;

User::User(void) : caps(false), capsend(false), pass(false), nick(false), user(false), welcome(false), welcome_done(false), change_mode(true), socket(-1) { str_buffer = "";}
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

std::map<std::string, Channel> User::getChannels() const
{
    return (this->channels);
}

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

void	User::clearBuffer(void) {
    str_buffer = "";
}

bool	User::isBufferReady(std::string to_add) {
    str_buffer += to_add;
    int size = str_buffer.size();
    if (str_buffer[size - 1] == '\n')
    {
	if (size > 2 && str_buffer[size - 2] != '\r')
	{
	    str_buffer[size - 1] = '\r';
	    str_buffer += '\n';
	}
	return (true);
    }
    return (false);
}

 void	User::sendClient(const std::string packet)
{
	std::string output(packet + "\r\n");
	std::cout  << BLUE << "-> " << packet << NORMAL;

	if (send(socket, output.c_str(), output.length(), 0) == -1)
		throw errorReturn(strerror(errno));
}

void	User::Registration(std::string packet, global global)
{
	(void)packet;
	if (!str_buffer.compare(0, 6, "CAP LS"))
	{
	//	std::string output =+ ":";
	//	output += SERVER_NAME;
	//	output += " 421 * CAP";
	//	sendClient( output);
	//	std::cout << "\n";
		caps = true;
        eraseCMD(&str_buffer);
		return ;
	}
	if(!str_buffer.compare(0, 6, "CAP END"))
	{
		capsend = true;
		return ;
	}
	// if (!str_buffer.compare(0, 4, "NICK"))// && pass == false)
	// {
	// 	eraseCMD(&str_buffer);
	// 	return ;
	// }
	if (!str_buffer.compare(0, 4, "NICK"))// && pass == true)
	{
		std::vector<std::string> extract;
		extract = ft_extract(str_buffer, ' ');
		if (extract.size() < 2)
		{
			sendClient(ERR_NEEDMOREPARAMS(this, str_buffer));
			eraseCMD(&str_buffer);
			return ;
		}
		commandNICK(str_buffer);
		nick = true;
         eraseCMD(&str_buffer);
		return ;
	}
	else if (!str_buffer.compare(0, 4, "PASS"))
	{
		std::vector<std::string> extract = ft_extract(packet, ' ');
		if (extract.size() < 2)
		{
			sendClient(ERR_NEEDMOREPARAMS(this, str_buffer));
			eraseCMD(&str_buffer);
			return ;
		}
		if (extract[1] != SSTR(global.password))
		{
			sendClient("464 :Password incorrect\r\n");
			if (nickname.size() > 1)
			{
				std::string output;
				output = ":!@ NICK :";
				output += nickname;
				output += "\r\n"; 
				sendClient(output);
			}
				eraseCMD(&str_buffer);
				throw badPassword();
		}
		pass = true;
		eraseCMD(&str_buffer);
		return ;
	}
    
     if (!str_buffer.compare(0, 4, "USER"))
	{
		std::vector<std::string> extract;
		extract = ft_extract(str_buffer, ' ');
		if (extract.size() < 2)
		{
			sendClient(ERR_NEEDMOREPARAMS(this, str_buffer));
			throw errorReturn(strerror(errno));
		}
		if (str_buffer.size() < 4)
		{
			throw errorReturn(strerror(errno));
		}
		commandUSER(str_buffer);
		user = true;
        eraseCMD(&str_buffer);
		return ;
	}
     std::cout << "Unknown command: " << str_buffer;
     eraseCMD(&str_buffer);
	//errorReturn(strerror(errno));
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
void	User::chooseCMD(std::map<int, User>user_tab, int j, global global)
{
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
		    	Registration(str_buffer, global);
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
			else if (!str_buffer.compare(0, 6, "NOTICE"))
			{
				commandPRIVMSG(str_buffer, user_tab, j);
				eraseCMD(&str_buffer);
			}
		    else if (!str_buffer.compare(0, 4,"motd"))
		    {
				commandMOTD(str_buffer);
				eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 5, "TOPIC"))
		    {
				commandTOPIC(str_buffer);
				eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 4, "LIST"))
		    {
				commandLIST(str_buffer);
				eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 6, "INVITE"))
		    {
				commandINVITE(str_buffer);
				eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 4, "QUIT"))
		    {
				commandQUIT(str_buffer, user_tab, j);
				eraseCMD(&str_buffer);
		    }
		    else if (!str_buffer.compare(0, 4, "KICK"))
		    {
			commandKICK(str_buffer);
			eraseCMD(&str_buffer);
		    }
		    else
		    {
				std::cout << BLUE << "Unknown command: "  << str_buffer << NORMAL << std::endl;
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

std::string  getPrefix(User &usr) {
    std::string	ret = usr.nickname;

    std::vector<std::string> ext = ft_extract(usr.username + "\r\n", ' ');
    return (ret + "!" + ext[0] + "@" + usr.ipaddr);
}

void	User::commandNICK(std::string &buffer)
{    
	std::string output;
	std::vector<std::string> extract = ft_extract(buffer, ' ');
	if (extract.size() < 2)
	{
		sendClient(ERR_NEEDMOREPARAMS(this, buffer));
		return;
	}
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
        it = allNickname.begin();
	while (it != allNickname.end())
        {
	    if (*it == nickname)
	    {
		*it = new_name;
		break;
	    }
	    ++it;
	}
	if (it == allNickname.end())
	    allNickname.push_back(new_name);
	output += nickname;
	output += " NICK ";
	output += new_name;
	output += "\r\n";
	
	//find and change into channel
	std::map<std::string, Channel>::iterator indexChannel;
	indexChannel = this->channels.begin();
	if (indexChannel != this->channels.end())
	{
		std::vector<t_client>::iterator it1 = indexChannel->second._chan_clients.begin();
		std::vector<t_client>::iterator itend = indexChannel->second._chan_clients.end();
		while (it1 != itend)
		{
			if (it1->nickname == nickname)
			{
				it1->nickname = new_name;
				break;
			}
			it1++;
		}
	}
	nickname = new_name;
    std::cout << GREEN  << "#   Serveur info: " << "Socket number# " << getSocket() << " set nickname to " << nickname << NORMAL << std::endl;
	sendClient(output);
}

void	User::commandUSER(std::string &buffer)
{
	std::vector<std::string> extract = ft_extract(buffer, ' ');
	if (extract.size() < 2)
	{
		sendClient(ERR_NEEDMOREPARAMS(this, buffer));
		return;
	}
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


std::string User::getMode(void)
{
	std::vector<std::string>::iterator it;
	std::string output;

	while (it != mode.end())
	{
		output.append(*it);
		it++;
	}
	return (output);
}
void	User::modeUser(std::string &buffer, int path, std::vector<std::string> extract)
{
	std::string returnMode = ":";
	if (path == 1)
	{
		std::vector<std::string>::iterator it = mode.begin();
		returnMode += SERVER_NAME;
		returnMode += " 221 ";
		returnMode += nickname;
		returnMode += " ";
		if (it != mode.end())
		{
			while (it != mode.end())
			{
				returnMode += *it;
				it++;
			}
			returnMode += "\r\n";
			sendClient(returnMode);
			return;
		}
		else
		{
			returnMode += "\r\n";
			sendClient(returnMode);
			return ;
		}
		
	}
	else if (path == 2)
	{
		if (extract[1] != nickname || extract[2].length() != 2)
		{
			sendClient(ERR_UNKNOWNCOMMAND(this, SERVER_NAME));
			return;
		}
	 	if ((extract[2][0] == '+')  && (extract[2][1] == 'i' || extract[2][1] == 'w'
	 	|| extract[2][1] == 'r'|| extract[2][1] == 'o'|| extract[2][1] == 'O'
	 	|| extract[2][1] == 's'))
	 	{
	 		//si on trouve le char dans notre vector
	 		std::string new_mode;
	 		new_mode.push_back(extract[2][1]);
	 		std::vector<std::string>::iterator find = mode.begin();
	 		while (find != mode.end())
	 		{
	 			if (new_mode == *find)
	 			{
	 				sendClient(ERR_UNKNOWNCOMMAND(this, SERVER_NAME));
	 				return;
	 			}
	 			find++;
	 		}
	 		mode.push_back(new_mode);
	 		std::string output = ":";
	 		output += getPrefix(*this);
	 		output += " MODE ";
	 		output += nickname;
	 		output += " :";
	 		output += new_mode;
	 		output += "\r\n";
	 		sendClient(output);
	 		return ;
	 	}
	 	else if ((extract[2][0] == '-')  && (extract[2][1] == 'i' || extract[2][1] == 'w'
	 	|| extract[2][1] == 'r'|| extract[2][1] == 'o'|| extract[2][1] == 'O'
	 	|| extract[2][1] == 's'))
	 	{

			std::string new_mode;
	 		new_mode.push_back(extract[2][1]);
	 		std::vector<std::string>::iterator find = mode.begin();
			while (find != mode.end())
	 		{
	 			if (new_mode == *find)
	 			{
	 				find = mode.erase(find);
					std::string output = ":";
	 				output += getPrefix(*this);
	 				output += " MODE ";
	 				output += nickname;
	 				output += " :";
				//	output += getMode();
	 				output += "\r\n";
	 				sendClient(output);
	 				return;
	 			}
	 			find++;
	 		}
			 //:flmastor!florianm@florianm MODE flmastor :-
			std::string output = ":";
	 		output += getPrefix(*this);
	 		output += " MODE ";
	 		output += nickname;
	 		output += " :-\r\n";
			sendClient(output);
	 		return ;		
	 	}
	 	else
	 	{
				sendClient(ERR_UMODEUNKNOWNFLAG(this, buffer));
				return ;
		}
	}
}

void	User::commandMODE(std::string &buffer)
{
	std::vector<std::string> extract = ft_extract(buffer, ' ');
	if (extract.size() < 2)
	{
		sendClient(ERR_NEEDMOREPARAMS(this, buffer));
	}
	else if(extract.size() == 2)
		modeUser(buffer, 1, extract);
	else if(extract.size() == 3)
		modeUser(buffer, 2, extract);
	else if (extract.size() == 4)
		commandModeChannel(buffer);
	else
		sendClient(ERR_UNKNOWNCOMMAND(this, SERVER_NAME));
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
    std::string JOINmsg = ":";
    JOINmsg += getPrefix(usr) + " JOIN :" + chan.getName() + "\r\n";
    chan.sendAllClient(JOINmsg);

    //RPL_TOPIC
    std::string RPL_TOPIC = ":";
    RPL_TOPIC += getPrefix(usr) + " 332 " + usr.nickname;
    RPL_TOPIC += " " + chan.getName() + " :" + chan.topic + "\r\n";
    usr.sendClient(RPL_TOPIC);

    //RPL_NAMERPL
    std::string RPL_NAMERPL = ":";
    std::vector<t_client>::iterator it = chan._chan_clients.begin();
    RPL_NAMERPL += getPrefix(usr) + " 353 " + usr.nickname;
    RPL_NAMERPL += " = " + chan.getName() + " :";
    while (it != chan._chan_clients.end())
    {
	RPL_NAMERPL += " ";
	if (chan.isOperator(it->nickname))
	    RPL_NAMERPL += "@";
	RPL_NAMERPL += it->nickname;
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
    std::vector<std::string> password;
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
	splitArg(password, buffer.substr(pos1, pos2 - pos1));
    }
    std::vector<std::string>::iterator it = chan.begin();
    std::vector<std::string>::iterator pass_it = password.begin();
    std::string pass = "";
    while (it != chan.end())
    {
	pass = (pass_it == password.end()) ? "" : *pass_it;
	if (!createOrJoin(channels, *it, user_tab, j, pass))
	    return ;
	if (!channels.find(*it)->second.namedCorrectly())
	{
	    channels.erase(*it);
	    std::cout << "Error: invalid argument" << std::endl;
	    return ;
	}
	JOINwelcome(*this, channels[*it]);
	++it;
	if (pass_it != password.end())
	    ++pass_it;
    }
}

bool User::chanExist(std::string &chan) {
    std::map<std::string, Channel>::iterator it = channels.begin();

    while (it != channels.end())
    {
	if (it->second.getName() == chan)
	    return (true);
	++it;
    }
    return (false);
}

bool User::inChan(std::string &nickname, std::string &chan) {
    std::vector<t_client>::iterator it = channels[chan]._chan_clients.begin();

    while (it != channels[chan]._chan_clients.end())
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

void	User::commandModeChannel(std::string &buffer) {
    std::vector<std::string> token = ft_extract(buffer, ' ');
    std::string err = "";

    if (token.size() < 4)
    {
	err += ":" + getPrefix(*this) + " 461 MODE :Not enough parameters";
	err += "\r\n";
	sendClient(err);
	return ;
    }

    std::map<std::string, Channel>::iterator it = channels.begin();
    while (it != channels.end())
    {
	if (it->first == token[1])
	    break ;
	++it;
    }
    if (it == channels.end())
    {
	err = ":" + getPrefix(*this) + " 403 " + token[1] + " :No such channel";
	err += "\r\n";
	sendClient(err);
	return ;
    }
    if (!inChan(token[3], token[1]))
    {
	err = ":" + getPrefix(*this) + " 441 " + token[3] + " ";
	err += token[1] + " :They aren't on that channel\r\n";
	sendClient(err);
	return ;
    }
    if (token[2][1] != 'o' || (token[2][0] != '+' && token[2][0] != '-'))
    {
	err = ":" + getPrefix(*this) + " 472 " + token[2][0];
	err += " :is unknown mode char to me for " + token[1] + "\r\n";
	sendClient(err);
	return ;
    }
    if (!channels[token[1]].isOperator(nickname))
    {
	err = ":" + getPrefix(*this) + " 482 " + nickname + " " + token[1];
	err += " :You're not channel operator\r\n";
	sendClient(err);
	return ;
    }
    if (token[2][0] == '+')
	channels[token[1]].addOperator(token[3]);
    else if (token[2][0] == '-')
	channels[token[1]].removeOperator(token[3]);
    std::string RPL_CHANNELMODEIS = ":" + getPrefix(*this) + " 324 " + nickname + " ";
    RPL_CHANNELMODEIS += token[1] + " " + token[2] + " " + token[3] + "\r\n";
    sendClient(RPL_CHANNELMODEIS);
}

void	User::commandTOPIC(std::string &buffer) {
    std::vector<std::string> token = ft_extract(buffer, ' ');
    std::string	err = "";

    if (token.size() == 1)
    {
	err += ":" + getPrefix(*this) + " 461 TOPIC :Not enough parameters";
	err += "\r\n";
	sendClient(err);
	return ;
    }
    std::map<std::string, Channel>::iterator it = channels.begin();
    while (it != channels.end())
    {
	if (it->first == token[1])
	    break ;
	++it;
    }
    if (it == channels.end())
    {
	err = ":" + getPrefix(*this) + " 403 " + token[1] + " :No such channel";
	err += "\r\n";
	sendClient(err);
	return ;
    }
    if (!inChan(nickname, token[1]))
    {
	err = ":" + getPrefix(*this) + " 442 ";
	err += token[1] + " :You're not on that channel\r\n";
	sendClient(err);
	return ;
    }
    if (token.size() > 2)
    {
	int pos = buffer.rfind(':', buffer.size()) + 1;
	std::string topic = buffer.substr(pos, buffer.size() - pos - 2);
	channels[token[1]].topic = topic;
    }
    std::string RPL_TOPIC = ":" + getPrefix(*this) + " 332 " + nickname;
    RPL_TOPIC += " " + token[1] + " :" + channels[token[1]].topic + "\r\n";
    channels[token[1]].sendAllClient(RPL_TOPIC);
}

void	User::commandLIST(std::string &buffer) {
    std::vector<std::string> token = ft_extract(buffer, ' ');
    std::string RPL_LIST = ":" + getPrefix(*this) + " 322 " + nickname;
    std::string RPL_LISTEND = ":" + getPrefix(*this) + " 323 " +nickname;

    if (token.size() < 2)
	return ;

    std::map<std::string, Channel>::iterator it = channels.begin();
    while (it != channels.end())
    {
	if (it->first == token[1])
	{
	    RPL_LIST += " " + it->second._name + " :" + it->second.topic + "\r\n";
	    RPL_LISTEND += " :End of LIST\r\n";
	    sendClient(RPL_LIST);
	    sendClient(RPL_LISTEND);
	}
	++it;
    }
}

void	User::commandINVITE(std::string &buffer) {
    std::vector<std::string> token = ft_extract(buffer, ' ');
    std::string err = "";

    if (token.size() < 3)
    {
	err += ":" + getPrefix(*this) + " 461 INVITE :Not enough parameters";
	err += "\r\n";
	sendClient(err);
	return ;
    }

    std::map<std::string, Channel>::iterator ite = channels.begin();
    while (ite != channels.end())
    {
	if (ite->first == token[2] && !inChan(nickname, token[2]))
	{
	    err += getPrefix(*this) + " 442 " + nickname + " " + token[2];
	    err += " :You're not on that channel\r\n";
	    sendClient(err);
	    return ;
	}
	++ite;
    }
    std::string RPL_INVITING = ":";
    RPL_INVITING += getPrefix(*this) + " 341 " + nickname + " ";
    RPL_INVITING += token[2] + " " + token[1] + "\r\n";
    sendClient(RPL_INVITING);
}

void	User::commandKICK(std::string &buffer) {
    std::vector<std::string> token = ft_extract(buffer, ' ');
    std::string err = "";
    std::string msg = "";

    if (token.size() < 3)
    {
	err = ":" + getPrefix(*this) + " 461 KICK :Not enough parameters";
	err += "\r\n";
	sendClient(err);
	return ;
    }
    if (!chanExist(token[1]))
    {
	err = ":" + getPrefix(*this) + " 403 " + token[1] + " :No such channel";
	err += "\r\n";
	sendClient(err);
	return ;
    }
    if (!channels[token[1]].isOperator(nickname))
    {
	err = ":" + getPrefix(*this) + " 482 " + nickname + " " + token[1];
	err += " :You're not channel operator\r\n";
	sendClient(err);
	return ;

    }
    if (!inChan(token[2], token[1]))
    {
	err = ":" + getPrefix(*this) + " 441 " + token[2] + " ";
	err += token[1] + " :They aren't on that channel\r\n";
	sendClient(err);
	return ;
    }
    std::cout << "B" << std::endl;
    if (token.size() > 3)
    {
	if (token[3][0] == ':')
	{
	    for (unsigned int i = 3; i < token.size(); i++)
		msg += token[i];
	}
    }
    std::cout << "A" << std::endl;
    std::string RPL_KICK = "";
    RPL_KICK = ":" + getPrefix(*this) + " KICK " + token[1];
    RPL_KICK += " " + token[2] + " " + msg + "\r\n";
    channels[token[1]].sendAllClient(RPL_KICK);
}

//////////////////////////////////
//End of channel related command//
/////////////////////////////////

void	User::commandPRIVMSG(std::string &buffer, std::map<int, User>	user_tab, int j)
{
	if (buffer.find('#') != std::string::npos || buffer.find('&') != std::string::npos)
		chanPRIVMSG(buffer, user_tab, j);
	else
		userPRIVMSG(buffer, user_tab, j);
}

void	User::chanPRIVMSG(std::string &buffer, std::map<int, User> user_tab, int j)
{
	if (channels.size() >= 1)
	{
		std::map<std::string, Channel>::iterator it = user_tab[j].channels.begin();

		for (std::vector<t_client>::iterator it1 = it->second._chan_clients.begin(); it1 != it->second._chan_clients.end(); it1++)
		{
			std::string tmp = ":";
			bool in_Chan = inChan(user_tab[j].nickname, it->second._name);

			tmp += getPrefix(user_tab[j]);
			tmp += " " + buffer + "\r\n";
			std::cout  << RED << "-> " << tmp << NORMAL;
			if (j != it1->socket && in_Chan == true)
				send(it1->socket, tmp.c_str(), tmp.size(), 0);
		}
	}
}

void	User::userPRIVMSG(std::string &buffer, std::map<int, User> user_tab, int j)
{
	if (user_tab.size() >= 1)
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

void	User::commandQUIT(std::string &buffer, std::map<int, User> user_tab, int j)
{
	int pos = 0;
	std::string tmp = ":";

	pos = buffer.find(':');
	tmp += getPrefix(user_tab[j]);
	tmp += " " + buffer.substr(0, pos);
	tmp += "\033[34m Ce zig est enfin partis !!!\033[0m"; // "NOUS POUVONS DIRE QUE C'EST UN LACHE"
	tmp += "\r\n";

	std::cout << YELLOW << tmp << NORMAL <<std::endl;
	if (user_tab.size() >= 1)
	{
		for (std::map<int, User>::iterator it_u = user_tab.begin(); it_u != user_tab.end(); it_u++)
		{
			if (j != it_u->second.socket)
				send (it_u->second.socket, tmp.c_str(), tmp.size(), 0);
		}
	}
}
