/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:48:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/23 22:55:42 by ccarrace         ###   ########.fr       */
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
{
}

Client::Client(const Client & source)
{
	_fd = source._fd;
	_isAuthenticated = source._isAuthenticated;
	_isRegistered = source._isRegistered;
	_clientBuffer = source._clientBuffer;
}

Client & Client::operator=(const Client & source)
{
    if (this != &source) {
        _fd = source._fd;
		_isAuthenticated = source._isAuthenticated;
        _isRegistered = source._isRegistered;
        _clientBuffer = source._clientBuffer;
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

std::string & Client::getBuffer()
{
	return(this->_clientBuffer);
}

void	Client::setNickname(const std::string & nickname)
{
	this->_nickname = nickname;
}

std::string	Client::getNickname()
{
	return (this->_nickname);
}

void	Client::setUsername(const std::string & username)
{
	this->_username = username;
}

std::string	Client::getUsername()
{
	return (this->_username);
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

// void Client::setBuffer(std::string buffer)
// {
//     this->_clientBuffer = buffer;
// }

// void Client::addBuffer(std::string& buffer)
// {
//     this->_clientBuffer += buffer;
// }
