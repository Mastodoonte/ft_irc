#include "include/Channel.hpp"
#include "include/user.hpp"

Channel::Channel(const std::string &src) : topic(""), _name(src), _password(""),  _isClosed(false)
{
}


Channel::Channel(const std::string &src, const std::string &password) : topic(""), _name(src), _password(password),  _isClosed(false)
{   
}

Channel::Channel(const Channel &src)
{
    _name = src._name;
}

Channel::Channel(void) : topic(""), _name(""), _password(""), _isClosed(false) {
}

Channel::~Channel(void) {
}

Channel Channel::operator=(const Channel &src)
{
    this->_name = src._name;
    this->_password = src._password;
    this->_isClosed = src._isClosed;
    this->_chan_invited = src._chan_invited;
    this->_chan_operators = src._chan_operators;
    this->_chan_clients = src._chan_clients;
    this->_chan_banned = src._chan_banned;
    this->topic = src.topic;
    return (*this);
}

void	Channel::sendAllClient(std::string packet) {
    std::vector<t_client>::iterator it = _chan_clients.begin();
    std::string output;

    while (it != _chan_clients.end())
    {
	output = packet + "\r\n";
	std::cout << BLUE << "-> " << packet << NORMAL;
	if (send(it->socket, output.c_str(), output.length(), 0) == -1)
		throw errorReturn(strerror(errno));
	++it;
    }
}

bool	Channel::isOperator(std::string &nickname) {
    std::vector<std::string>::iterator it = _chan_operators.begin();

    while (it != _chan_operators.end())
    {
	if (*it == nickname)
	    return (true);
	++it;
    }
    return (false);
}

void	Channel::addOperator(std::string &nickname) {
    if (!isOperator(nickname))
	_chan_operators.push_back(nickname);
}

void	Channel::removeOperator(std::string &nickname) {
    std::vector<std::string>::iterator it = _chan_operators.begin();

    while (it != _chan_operators.end())
    {
	if (*it == nickname)
	{
	    _chan_operators.erase(it);
	    return ;
	}
	++it;
    }
}

std::string Channel::getName(void) {
    return (_name);
}

std::vector<t_client>   Channel::getChanClient()
{
    return (_chan_clients);
}

/*
 * Channels names are strings (beginning with a '&', '#', '+' or '!'
 * character) of length up to fifty (50) characters.  Channel names are
 * case insensitive.
 * 
 * Apart from the the requirement that the first character being either
 * '&', '#', '+' or '!' (hereafter called "channel prefix"). The only
 * restriction on a channel name is that it SHALL NOT contain any spaces
 * (' '), a control G (^G or ASCII 7), a comma (',' which is used as a
 * list item separator by the protocol).  Also, a colon (':') is used as
 * a delimiter for the channel mask.  The exact syntax of a channel name
 * is defined in "IRC Server Protocol" [IRC-SERVER].
 * 
 * The use of different prefixes effectively creates four (4) distinct
 * namespaces for channel names.  This is important because of the
 * protocol limitations regarding namespaces (in general)
 * */
bool Channel::namedCorrectly(void)
{
    std::size_t found;
    std::string parameters;
    std::string chan_name = _name;

    parameters = chan_name.substr(0, 1);
    if (parameters != "&" && parameters != "#" && parameters != "+" && parameters != "!")
	return (false);
    found = chan_name.find(" ");
    if (found != std::string::npos)
        return(false);
    found = chan_name.find("^G");
    if (found != std::string::npos)
        return(false);
    found = chan_name.find(",");
    if (found != std::string::npos)
        return(false);
    found = chan_name.find(":");
    if (found != std::string::npos)
        return(false);
    if(chan_name.size() > 50)
        return (false);
    return (true);
}

std::string Channel::getPassword(void) {
    return (_password);
}

void	    Channel::setPassword(std::string password) {
    _password = password;
}

/*
 * Here the objective is to create a channel and add it right into our map channels_list
 * if the channel doesn't exit, otherwise we return the pointer of the channel
 * */
bool	createOrJoin(std::map<std::string, Channel> &channels_list, std::string chan_name, std::map<int, User>    user_tab, int j, std::string password)
{
    std::map<std::string, Channel>::iterator it = channels_list.begin();
    std::string err = "";
    while (it != channels_list.end())
    {
        if (it->first == chan_name)
        {
	    if (it->second.getPassword() != password)
	    {
		err += ":" + getPrefix(user_tab[j]) + " 475 " + it->first;
		err += " :Cannot join channel (+k)\r\n";
		user_tab[j].sendClient(err);
		return (false);
	    }
            t_client tmp1;
            tmp1.socket = user_tab[j].socket;
            tmp1.nickname = user_tab[j].nickname;
            it->second._chan_clients.push_back(tmp1);
            return (true);
        }
        it++;
    }
    Channel new_channel(chan_name);
    t_client tmp;
    tmp.socket = user_tab[j].socket;
    tmp.nickname = user_tab[j].nickname;
    new_channel._chan_clients.push_back(tmp);
    new_channel.addOperator(user_tab[j].nickname);
    if (password != "")
	new_channel.setPassword(password);
    channels_list[chan_name] = new_channel;
    return true;
}
