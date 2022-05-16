#include "include/Channel.hpp"


Channel::Channel(const std::string &src) : _name(src), _password(NULL), _isClosed(false)
{
    ;
}


Channel::Channel(const std::string &src, const std::string &password) : _name(src), _password(password), _isClosed(false)
{
    ;
}

Channel::Channel(const Channel &src)
{
    _name = src._name;
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
    return (*this);
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
bool Channel::namedCorrectly(const std::string &chan_name)
{
    std::size_t found;
    std::string parameters;

    parameters = chan_name.substr(0, 1);
    if (parameters != "&" || parameters != "#" || parameters != "+" || parameters != "!")
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

/*
 * Here the objective is to create a channel and add it right into our map channels_list
 * if the channel doesn't exit, otherwise we return the pointer of the channel
 * */
Channel *Channel::createOrJoin(std::map<std::string, Channel*> channels_list, std::string chan_name)
{
    std::map<std::string, Channel*>::iterator it = channels_list.begin();
    while (it != channels_list.end())
    {
        if (it->first == chan_name)
            return (it->second);
        it++;
    }
    Channel *new_channel = new Channel(chan_name);
    channels_list[chan_name] = new_channel;
    return new_channel;
}