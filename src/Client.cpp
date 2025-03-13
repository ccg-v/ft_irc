/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:48:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/12 20:04:53 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* --- Public Coplien's functions ------------------------------------------- */

Client::Client() : _fd(-1) // Default constructor
{
}

Client::Client(int fd) // Parameterized constructor
	: _fd(fd),
	  _isAuthenticated(false), 
	  _isRegistered(false),
	  _clientBuffer("")
//	  _maxChannels(MAXCHAN)
{
//	std::cout << "\n[~DEBUG]: client created with sockefd: " << _fd << " and maxChannels: " << _maxChannels << std::endl;
}

Client::Client(const Client &source)
{
	_fd = source._fd;
	_isAuthenticated = source._isAuthenticated;
	_isRegistered = source._isRegistered;
	_clientBuffer = source._clientBuffer;
	_subscriptions = source._subscriptions;
}

Client &Client::operator=(const Client &source)
{
    if (this != &source)
	{
        _fd = source._fd;
		_nickname = source._nickname;
		_username = source._username;
		_isAuthenticated = source._isAuthenticated;
		_isRegistered = source._isRegistered;
		_clientBuffer = source._clientBuffer;
	//	_maxChannels = source._maxChannels;
		// _channels = source._channels;
		_subscriptions = source._subscriptions;
    }
    return *this;
}

Client::~Client()	// Default destructor
{
}

/* --- Getters and Setters -------------------------------------------------- */

void	Client::setFd(int fd)
{
	this->_fd = fd;
}

int		Client::getFd()
{
	return(this->_fd);
}

void	Client::setClientIp(std::string clientIp)
{
	this->_clientIp = clientIp;
}

std::string	Client::getClientIp()
{
	return(this->_clientIp);
}

std::string	&Client::getBuffer()
{
	return(this->_clientBuffer);
}

void	Client::setNickname(const std::string &nickname)
{
	this->_nickname = nickname;
}

std::string	Client::getNickname()
{
	return (this->_nickname);
}

void	Client::setUsername(const std::string &username)
{
	this->_username = username;
}

std::string	Client::getUsername()
{
	return (this->_username);
}

void	Client::setRealname(const std::string & realname)
{
	this->_realname = realname;
}

std::string	Client::getRealname()
{
	return (this->_realname);
}

void	Client::setHostMask()
{
	this->_hostMask = this->getNickname() + "!" + this->getUsername() + "@" + this->getClientIp();
}

std::string	Client::getHostMask()
{
	return (this->_hostMask);
}

void	Client::setAuthentication(bool isAuthenticated)
{
	this->_isAuthenticated = isAuthenticated;
}

bool	Client::getAuthentication()
{
	return (this->_isAuthenticated);
}

// int Client::getChannelCnt(void) const
// {
//     return (this->_channels.size());
// }

// int Client::getMaxChannels(void) const
// {
//     return (this->_maxChannels);
// }

void	Client::setRegistration(bool isRegistered)
{
	this->_isRegistered = isRegistered;
}

bool	Client::getRegistration()
{
	return (this->_isRegistered);
}

std::map<Channel*, bool>& Client::getSubscriptions()
{
	return (this->_subscriptions);
}

// OTHER METHODS

void Client::addSubscription(Channel *channel, bool isChanOp)
{
    this->_subscriptions[channel] = isChanOp; // [1]
	// for (std::map<std::string, bool>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	// {
    // 	std::cout << "[~DEBUG]: one channel is: " << it->first << std::endl;
	// }
}

// void Client::_sendMessage(const std::string &message) const
// {
//     std::string msg = message + "\r\n";
//     send(this->_fd, msg.c_str(), msg.length(), 0);
// }
// void Client::setBuffer(std::string buffer)
// {
//     this->_clientBuffer = buffer;
// }

// void Client::addBuffer(std::string& buffer)
// {
//     this->_clientBuffer += buffer;
// }

/*	[1] You can add items to a std::map<Key, Value> using these methods:
	1. Using operator[] (Creates or Updates)

	std::map<std::string, int> myMap;
	myMap["apple"] = 5;  // Inserts ("apple", 5)
	myMap["banana"] = 10; // Inserts ("banana", 10)
*/

void	Client::unsubscribe(Channel &channel)
{
	std::map<Channel*, bool>::iterator it = _subscriptions.find(&channel);
	
	if (it != _subscriptions.end())
	{
		_subscriptions.erase(it);
	}
	else
	{
		std::cout << "[~DEBUG]: Client is not subscribed to that channel" << std::endl;
	}
}

