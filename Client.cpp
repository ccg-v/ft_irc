/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:48:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/16 14:28:12 by ccarrace         ###   ########.fr       */
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
	  _clientBuffer(NULL)
{
}

Client::Client(const Client & source) : _fd(source._fd) {}

Client& Client::operator=(const Client & source)
{
    if (this != &source) {
        _fd = source._fd;
    }
    return *this;
}

Client::~Client()	// Default destructor
{
}

/* --- Getters and Setters -------------------------------------------------- */

std::string & Client::getBuffer()
{
	return(_clientBuffer);
}

void	Client::setNickname(const std::string & nickname)
{
	this->_nickname = nickname;
}

std::string	Client::getNickname()
{
	return (this->_nickname);
}

void	Client::setAuthentication(bool isAuthenticated)
{
	this->_isAuthenticated = isAuthenticated;
}

bool	Client::getAuthentication()
{
	return (this->_isAuthenticated);
}

// void Client::setBuffer(std::string buffer)
// {
//     this->_clientBuffer = buffer;
// }

// void Client::addBuffer(std::string& buffer)
// {
//     this->_clientBuffer += buffer;
// }