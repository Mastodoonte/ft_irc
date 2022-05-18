#include "include/user.hpp"
#include "include/rpl.hpp"
#include <iomanip>
#include <sstream>
#include <string.h>
#include <errno.h>

User::User(void) : caps(false), capsend(false), pass(false), nick(false), user(false), welcome(false), socket(-1) {}

User::~User(void) {
}

User	&User::operator=(const User &cp) {
    if (this == &cp)
	return (*this);
    *this = cp;
    return (*this);
}

int	User::acceptUsr(int server_socket) {
    std::string RPL_WELCOME = "001";

    socket_len = sizeof(addr);
    socket = accept(server_socket, (struct sockaddr *)&addr, &socket_len);
    if (socket == -1)
    {
	std::cout << "Error to accept client" << std::endl;
	return (-1);
    }
    std::cout <<GREEN << "Serveur info: Un nouveau client tente de se connecter : " << inet_ntoa(addr.sin_addr) << " Avec pour socket : " << socket << NORMAL << std::endl;
    send(socket, RPL_WELCOME.c_str(), RPL_WELCOME.size(), 0);
    return (0);
}

//Lors de la connection le client envoi 3 commande
//dans un seul buffer, donc dès qu'une commande
//a été éffectuer je la retire du buffer.

static void eraseCMD(std::string &buffer) {
    int pos = 0;

    pos = buffer.find('\n');

    if (pos != -1)
	buffer.erase(0, pos + 1);
    else
	buffer.erase(0, buffer.size());
}

void	User::sendClient(const std::string &packet)
{
	std::string output(packet + "\r\n");
	std::cout << "-> " << packet << std::endl;

	if (send(socket, output.c_str(), output.length(), 0) == -1)
		throw errorReturn(strerror(errno));
}

void	User::Registration(std::string packet)
{
    std::cout << "on arrive bien la \n";
    std::cout << "Value = " << packet << "\n";
    if (!packet.compare(0, 6, "CAP LS"))
	{
		sendClient("CAP * LS :");
		caps = true;
        eraseCMD(packet);
		return ;
	}
	if(!packet.compare(0, 6, "CAP END"))
	{
		capsend = true;
		return ;
	}

	if (!packet.compare(0, 4, "NICK"))
	{
		commandNICK(packet);
		nick = true;
        eraseCMD(packet);
		return ;
	}
	
	else if (!packet.compare(0, 4, "PASS"))
	{
		if (packet.size() < 2)
			throw errorReturn(strerror(errno));
		pass = true;
        eraseCMD(packet);
		return ;
	}
    
     if (!packet.compare(0, 4, "USER"))
	{
		if (packet.size() < 4)
			throw errorReturn(strerror(errno));
		user = true;
        eraseCMD(packet);
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
    std::string str_buffer(buffer);
    std::cout << RED  << "Packet received from client: " << buffer << NORMAL << std::endl;
    while (str_buffer.size())
    {
        if (!str_buffer.compare(0, 4, "PING"))
		    return (PING(this, str_buffer));
	    if (checkIfRegistred() == false && welcome == false)
	    {
	    	Registration(str_buffer);
	    	if (caps == false)
	    		return ;
	    	if (pass == false)
	    		return ;
	    	if (nick == false)
	    		return ;
	    	if (user == false)
	    		return ;
	    	welcome = true;
	    	sendClient(RPL_WELCOME(this, "serveur_test"));
	    	sendClient(RPL_YOURHOST(this));
            eraseCMD(str_buffer);
	    	return ;
        }
	    /*if (!str_buffer.compare(0, 3, "CAP"))
	    {
	        commandCAP(str_buffer);
	        eraseCMD(str_buffer);
	    }
	    else if (!str_buffer.compare(0, 4, "NICK"))
	    {
	        commandNICK(str_buffer);
	        eraseCMD(str_buffer);
	    }
	    else if (!str_buffer.compare(0, 4, "USER"))
	    {
	        commandUSER(str_buffer);
	        eraseCMD(str_buffer);
	    }
	    else if (!str_buffer.compare(0, 4, "PONG"))
	    {
	        eraseCMD(str_buffer);
	    }
        else if (!str_buffer.compare(0, 4, "PASS"))
	    {
	        commandPASS(str_buffer);
	        eraseCMD(str_buffer);
	    }
        else if (!str_buffer.compare(0, 4, "MODE"))
	    {
	        commandMODE(str_buffer);
	        eraseCMD(str_buffer);
	    }*/
	    else
        {
	        std::cout << "Unknown command: " << str_buffer << std::endl;
        }
    }
}

void    send_to_client(int rpl_number, std::string reply, std::string nickname, int socket)
{
	std::ostringstream packet;

	packet	<< ":irc_serv "<< std::setfill('0') << std::setw(3) << rpl_number<< " " << nickname << " " << reply;
	std::string msg = packet.str();
	std::cout << BLUE << "Packet send to the client: " << "msg = "<< msg << NORMAL;
	if ( send(socket, &msg[0], msg.size(), 0) == -1 )
        throw errorReturn(strerror(errno));
}

int	User::getSocket(void) {
 return (socket);
}

void	User::clear(void) {
    close(socket);
}

sockaddr_in	&User::getAddr(void) {
    return (addr);
}

//////////////////////////////////////////////
/*                  Commands                */
//////////////////////////////////////////////
void	User::commandCAP(std::string &buffer)
{
    (void)buffer;
}

void	User::commandNICK(std::string &buffer)
{

    int pos1 = 0;
    int pos2 = 0;
    int welcome = 0;
    
    if (nickname == "" && username != "")
	welcome = 1;
    pos1 = buffer.find(' ') + 1;
    pos2 = buffer.find('\n');
    nickname = buffer.substr(pos1, pos2 - pos1);
    std::cout << GREEN  << "Serveur info: " << "Socket number# " <<getSocket() << " set nickname to " << nickname << NORMAL << std::endl;
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
    std::cout << GREEN << "Serveur info: " << "Socket number# " <<getSocket() << " set Username to " << username << NORMAL << std::endl;
}

void	User::commandPASS(std::string &buffer)
{
    (void)buffer;
}

void	User::commandMODE(std::string &buffer)
{
    (void)buffer;
}