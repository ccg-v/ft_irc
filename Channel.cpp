#include "Channel.hpp"

Channel::Channel(void) {}

Channel::Channel(std::string name) : _name(name)
{
	std::cout << "\n[~DEBUG]: new channel created named: " << _name << std::endl;
    /*this->_modes['i'] = false;
    this->_modes['t'] = false;
    this->_modes['k'] = false;
    this->_modes['o'] = false;
    this->_modes['l'] = false;*/
}

// void Channel::setTopic(std::string topic)
// {
//     this->_topic = topic;
// }

// void Channel::setKey(std::string key)
// {
//     this->_key = key;
// }

// void Channel::setMode(char mode, bool status)
// {
//     this->_modes[mode] = status;
// }

std::string Channel::getName(void) const
{
    return (this->_name);
}

// std::string Channel::getTopic(void) const
// {
//     return (this->_topic);
// }

// std::string Channel::getKey(void) const
// {
//     return (this->_key);
// }

// bool Channel::getMode(char mode)
// {
//     bool status = this->_modes[mode];
//     return (status);
// }

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

void Channel::addClient(int& fd)
{
    this->_clients.push_back(fd);

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
//         client.sendMessage(RPL_NAMREPLY(client.getNickname(), this->_name, "", this->_clients[i].getNickname()));
//     }
//     client.sendMessage(RPL_ENDOFNAMES(client.getNickname(), this->_name));
// }
