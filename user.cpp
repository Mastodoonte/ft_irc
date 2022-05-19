#include "include/user.hpp"
#include "include/rpl.hpp"
#include <iomanip>
#include <sstream>
#include <string.h>
#include <errno.h>
#define SERVER_NAME "TEST_SERVEUR"

User::User(void) : caps(false), capsend(false), pass(false), nick(false), user(false), welcome(false), socket(-1) {}

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

    socket_len = sizeof(addr);
    socket = accept(server_socket, (struct sockaddr *)&addr, &socket_len);
    if (socket == -1)
    {
		std::cout << "Error to accept client" << std::endl;
		return (-1);
    }
    std::cout <<GREEN << "#   Serveur info: Un nouveau client tente de se connecter : " << inet_ntoa(addr.sin_addr) << " Avec pour socket : " << socket << NORMAL << std::endl;
    return (0);
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
void	User::chooseCMD(char *buffer)
{
    str_buffer = std::string(buffer);
    while (str_buffer.size())
	{
		while (str_buffer.size())
    	{
			displayCmd(str_buffer);
    	    if (!str_buffer.compare(0, 4, "PING"))
			{
			    return (RPL_PING(this, str_buffer));
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

void	User::commandNICK(std::string &buffer)
{

    int pos1 = 0;
    int pos2 = 0;
    
    if (nickname == "" && username != "")
    pos1 = buffer.find(' ') + 1;
    pos2 = buffer.find('\n');
    nickname = buffer.substr(pos1, pos2 - pos1);
    std::cout << GREEN  << "#   Serveur info: " << "Socket number# " <<getSocket() << " set nickname to " << nickname << NORMAL << std::endl;
}

void	User::commandUSER(std::string &buffer) {
    int pos1 = 0;
    int pos2 = 0;
    int	welcome = 0;

    if (username == "" && nickname != "")
	welcome = 1;
    pos1 = buffer.find(' ') + 1;
    pos2 = buffer.find(' ', pos1);
    username = buffer.substr(pos1, pos2 - pos1);
    pos1 = buffer.find(':') + 1;
    realname = buffer.substr(pos1, buffer.size() - pos1);
    std::cout << GREEN << "#   Serveur info: " << "Socket number# " <<getSocket() << " set Username to " << username << NORMAL << std::endl;
}

void	User::commandPASS(std::string &buffer)
{
    (void)buffer;
}

void	User::commandMODE(std::string &buffer)
{
	(void)buffer;
	int pos1 = 0;
    int pos2 = 0;

	pos1 = buffer.find(' ') + 1;
    pos2 = buffer.find('\n');
    std::string mode = buffer.substr(pos1, pos2 - pos1);
	sendClient( RPL_MODE(this, SERVER_NAME));
}

/*
  <=======]}======
    --.   /|
   _\"/_.'/
 .'._._,.'
 :/ \{}/
(L  /--',----._
    |          \\
   : /-\ .'-'\ / |
--> \\, ||    \|
     \/ ||    ||*/