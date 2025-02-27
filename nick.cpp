/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:52:25 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/27 20:40:19 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO-DO:
// 	-	when a client changes his nick within a channel, all connected clients
//		should be noticed

/* See RFC 2812, Section 3.1.2 */

#include "Server.hpp"
#include "Client.hpp"

void 	Server::handleNick(Client &client, const t_tokens msgTokens)
{
	
	// Password not given, authentication pending
	if(!client.getAuthentication()) 
	{
		sendMessage(client, ERR_PASSWDMISMATCH(this->_serverName, client.getNickname()));
		return;
	}

	// Ensure a nickname was provided
	if (msgTokens.parameters.empty())
	{
		sendMessage(client, ERR_NONICKNAMEGIVEN(this->_serverName));
		return;
	}

	std::string newNick = msgTokens.parameters[0];

	// Validate new nickname
	if (!isNickValid(newNick))
	{
		sendMessage(client, ERR_ERRONEUSNICKNAME(this->_serverName, newNick));
		return;
	}

	// Check if nickname is already in use
	if (isNickTaken(newNick))
	{
		sendMessage(client, ERR_NICKNAMEINUSE(this->_serverName, client.getNickname(), newNick));
		return;	
	}

	// Handle nickname change
	if(!client.getNickname().empty())
	{
		sendMessage(client, INF_NICKCHANGED(client.getNickname(), client.getUsername(), client.getClientIp(), newNick));
		client.setNickname(newNick);
		return;
	}

	client.setNickname(newNick);

	// If the username is missing, notify the client
	if(client.getUsername().empty())
	{
		sendMessage(client, NTC_USERMISSING(this->_serverName, client.getUsername()));
		return;
	}
	
	// Complete registration and send welcome messages
	client.setRegistration(true);
	sendMessage(client, RPL_WELCOME(this->_serverName, client.getNickname(), client.getUsername(), client.getClientIp()));
	sendMessage(client, RPL_YOURHOST(this->_serverName, client.getNickname()));
	sendMessage(client, RPL_CREATED(this->_serverName, client.getNickname()));
	sendMessage(client, RPL_MYINFO(this->_serverName, client.getNickname()));
}

/*
 *	isNickValid()
 *
 *	- First character: only from ascii 65 to ascii 125 allowed
 *	- Rest of character: only alphanumeric or hyphen (-) allowed
 *	- Nickname length is limited to 9 characters (some servers allow up to 30)
 *
 *	(see RFC 2812, Section 2.3.1)
 */

bool Server::isNickValid(const std::string &nick)
{
	if (nick.length() > 9) // Adjust max length per server rules
		return (false);

	std::string specialStart = "[]\\^_{}|"; // Allowed special chars at the start
 
	// First character must be a letter (A-Z, a-z) or an allowed special character
	if (!std::isalpha(nick[0]) && specialStart.find(nick[0]) == std::string::npos)
		return (false);

	// Remaining characters can only be letters, digits, or hyphen
	for (size_t i = 1; i < nick.length(); i++)
	{
		char c = nick[i];
		if (!std::isalnum(c) && c != '-')
			return (false);
	}

	return (true);
}

bool	Server::isNickTaken(const std::string &nick)
{
	std::map<int, Client>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second.getNickname() == nick)
			return (true);
	}
	return (false);
}

