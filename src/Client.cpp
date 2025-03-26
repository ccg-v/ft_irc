/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:48:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/26 19:06:05 by erosas-c         ###   ########.fr       */
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
	  _passErrSent(false),
	  _clientBuffer(""),
	  _startTime(std::time(0)),
	  _nickValid(false)
{
}

Client::Client(const Client &source)
{
	_fd = source._fd;
	_isAuthenticated = source._isAuthenticated;
	_isRegistered = source._isRegistered;
	_passErrSent = source._passErrSent;
	_clientBuffer = source._clientBuffer;
	_startTime = source._startTime;
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
		_passErrSent = source._passErrSent;
		_clientBuffer = source._clientBuffer;
		_channels = source._channels;
		_startTime = source._startTime;
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

void	Client::setRegistration(bool isRegistered)
{
	this->_isRegistered = isRegistered;
}

bool	Client::getRegistration()
{
	return (this->_isRegistered);
}

void	Client::setPassErrSent(bool passErrSent)
{
	this->_passErrSent = passErrSent;
}

bool	Client::getPassErrSent()
{
	return (this->_passErrSent);
}

std::map<std::string, bool>	&Client::getChannels()
{
	return (this->_channels);
}

time_t	Client::getStartTime()
{
	return (this->_startTime);
}

// OTHER METHODS

bool	Client::isOperator(const std::string &ch_name)
{
	if (this->_channels[ch_name] == true)
		return (true);
	return (false);
}

void	Client::addChannel(std::string &channel, bool isChanOp)
{
    this->_channels[channel] = isChanOp; // [1]
}

bool	Client::isSubscriber(const std::string &channelName)
{
	std::map<std::string, bool>::iterator it = this->_channels.find(channelName);

	if (it != this->_channels.end())
		return (true);
	return (false);
}

void	Client::unsubscribe(std::string channelName)
{
	std::map<std::string, bool>::iterator it = this->_channels.find(channelName);
	
	if (it != this->_channels.end())
	{
		this->_channels.erase(it);
	}
}

void	Client::setNickValid(bool status)
{
	this->_nickValid = status;
}

bool	Client::getNickValid()
{
	return (this->_nickValid);
}
