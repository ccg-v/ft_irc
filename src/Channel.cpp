#include "Channel.hpp"

Channel::Channel(void) {}

//AFEGIR PARAMETRE KEY i mirar requisits de la KEY, tot allo de la x, etc.
Channel::Channel(std::string name) : _name(name)//, _key(key), _limit(BACKLOG), _topic(""), _ionly(false)
{
	//std::cout << "\n[~DEBUG]: new channel created named: " << _name << std::endl;
    this->_modes['i'] = false;
    this->_modes['t'] = false; // NO CALDRIA
    this->_modes['k'] = false; // NO CALDRIA
    this->_modes['o'] = false; // CREC QUE NO CALDRIA - MIRAR SI CAL O NO
    this->_modes['l'] = false; // NO CALDRIA
    this->_key = ""; // bona practica inicialitzar la key segons la mestra
    this->_limit = BACKLOG;
}

Channel::Channel(const Channel &src)
{
	_name = src._name;
	_key = src._key;
	_modes = src._modes;
    _clients = src._clients;
	_limit = src._limit;
}

Channel &Channel::operator=(const Channel &src)
{
    if (this != &src)
    {
        _name = src._name;
	    _key = src._key;
	    _modes = src._modes;
        _clients = src._clients;
	    _limit = src._limit;
    }
    return (*this);
}

// void Channel::setTopic(std::string topic)
// {
//     this->_topic = topic;
// }

void Channel::setKey(std::string key)
{
    this->_key = key;
    this->_modes['k'] = true; // no se si es pot fer o hem de fer servir un setMode a part, o si pot ser un problema en cas que ja sigui true (si es aixi, sempre podem posar un if)
}

void Channel::setMode(char mode, bool status)
{
    this->_modes[mode] = status;
 //   std::cout << "[DEBUG]: status: " << status << std::endl;
   // std::cout << "[DEBUG]: this->mode[i]: " << this->_modes['i'] << std::endl;
    if (mode == 'k' && status == false)
        this->_key = "";
}

void Channel::setLimit(int limit) // COMPROVAR QUE SIGUI > 0
{
    this->_limit = limit;
    this->_modes['l'] = true;
}

std::string Channel::getName(void) const
{
    return (this->_name);
}

// std::string Channel::getTopic(void) const
// {
//     return (this->_topic);
// }

std::string Channel::getKey(void) const
{
    return (this->_key);
}

bool Channel::getMode(char mode) const // [1]
{
    std::map<char, bool>::const_iterator it = this->_modes.find(mode);
    if (it != this->_modes.end())
    {
      //  std::cout << "[DEBUG]: getMode ft " << mode << " mode is: " << it->second << std::endl;
        return (it->second);
    }
    return (false);
}

std::vector<int> Channel::getClients(void) const
{
// #ifdef DEBUG
//     std::cout << "CLIENTS IN GETCLIENTS: ";
//     for (size_t i = 0; i < this->_clients.size(); i ++)
//     {
//         std::cout << this->_clients[i] << ", ";
//     }
//     std::cout << std::endl;
// #endif
    return (this->_clients);
}

void Channel::addClient(const int &fd)
{
    this->_clients.push_back(fd);
	//std::cout << "Client with fd " << fd << " added to channel named: " << this->_name << std::endl;
	// for (size_t i = 0; i < this->_clients.size(); ++i)
	// {
	// 	std::cout << "[~DEBUG]: channel client[" << i << "] fd: " << this->_clients[i] << std::endl;
	// }
// #ifdef DEBUG
//     std::cout << "CLIENTS IN ADDCLIENT: ";
//     for (size_t i = 0; i < this->_clients.size(); i ++)
//     {
//         std::cout << this->_clients[i] << ", ";
//     }
//     std::cout << std::endl;
// #endif
}

// void Channel::rplNamesList(Client& client)
// {
//     int n = this->_clients.size();

//     for (int i = 0; i < n; i ++)
//     {
//         client._sendMessage(RPL_NAMREPLY(client.getNickname(), this->_name, "", this->_clients[i].getNickname()));
//     }
//     client._sendMessage(RPL_ENDOFNAMES(client.getNickname(), this->_name));
// }

/*  [1] Cannot use "return (this->_modes[mode])" because the operator[] on std::map
        is not a const function and it can modify the map by inserting a default value
        if the key does not exist.
        If we want the getMode function to be const, _modes is treated as a
        const std::map<char, bool>, and SO operator[] cannot be used.
*/
