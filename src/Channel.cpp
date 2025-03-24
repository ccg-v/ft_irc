#include "Channel.hpp"

Channel::Channel(void) {}

Channel::Channel(std::string name, std::string key)
	: _name(name),
	  _topic(""),
	  _key(key),
	  _ionly(false),
	  _tmode(true),
	  _limit(0)
{
}

Channel::Channel(const Channel &src)
{
	_name = src._name;
	_key = src._key;
    _clients = src._clients;
	_limit = src._limit;
    _topic = src._topic;
    _ionly = src._ionly;
    _tmode = src._tmode;
	_whoSetTopic = src._whoSetTopic;
}

Channel &Channel::operator=(const Channel &src)
{
    if (this != &src)
    {
        _name = src._name;
	    _key = src._key;
        _clients = src._clients;
	    _limit = src._limit;
        _topic = src._topic;
        _ionly = src._ionly;
        _tmode = src._tmode;
		_whoSetTopic = src._whoSetTopic;
    }
    return (*this);
}

void Channel::setKey(std::string key)
{
    this->_key = key;
}

void Channel::setLimit(int limit)
{
    this->_limit = limit;
}

void Channel::setIonly(bool status)
{
    this->_ionly= status;
}

void Channel::setTmode(bool status)
{
    this->_tmode= status;
}

std::string Channel::getName(void) const
{
    return (this->_name);
}

int Channel::getLimit(void) const
{
    return (this->_limit);
}

std::string Channel::getTopic(void) const
{
    return (this->_topic);
}

std::string Channel::getKey(void) const
{
    return (this->_key);
}

bool Channel::getTmode() const
{
    return (_tmode);
}

bool Channel::getIonly() const
{
    return (_ionly);
}

std::vector<int> Channel::getClients(void) const
{
    return (this->_clients);
}

std::vector<int>& Channel::getClientsByRef()
{
	return (this->_clients);
}

void	Channel::setClients(const std::vector<int>& newClients)
{
	this->_clients = newClients;
}

std::vector<int> &Channel::getInvited(void)
{
    return (this->_invited);
}

void Channel::addClient(const int &fd)
{
    this->_clients.push_back(fd);
}

void	Channel::setTopic(const std::string & topic)
{
	this->_topic = topic;
}

std::string	Channel::getTopic()
{
	return (this->_topic);
}

void	Channel::setTopicAuthor(const std::string & nickname)
{
	this->_whoSetTopic = nickname;
}

std::string	Channel::getWhoSetTopic()
{
	return (_whoSetTopic);
}

bool	Channel::isMember(int fd)
{	
	for (size_t i = 0; i < this->_clients.size(); ++i)
	{
		if (this->_clients[i] == fd)
			return (true);
	}
	return (false);
}

void	Channel::removeMember(int fd)
{
    std::vector<int>::iterator it;

	it = std::find(this->_clients.begin(), this->_clients.end(), fd);
    if (it != this->_clients.end())
	{
		this->_clients.erase(it);
	}
}

void Channel::addInvite(const int fd)
{
	this->_invited.push_back(fd);
}
