/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:55:20 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/27 21:43:44 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

/* See RFC 2812, Section 3.1.3 */

void 	Server::handleUser(Client &client, const t_tokens msgTokens)
{
	if(!client.getAuthentication())
	{
		sendMessage(client, ERR_PASSWDMISMATCH(this->_serverName, client.getNickname()));
		return;
	}

	if(client.getRegistration())
	{
		sendMessage(client, ERR_ALREADYREGISTERED(this->_serverName));
		return;
	}

	if (msgTokens.parameters.size() != 3 || msgTokens.trailing.empty())
	{
		sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return;
	}

	// Validate username
	if (!isUserValid(msgTokens.parameters[0])) 
	{
		sendMessage(client, ERR_INVALIDUSERNAME(this->_serverName, msgTokens.parameters[0]));
		return;
	}

	client.setUsername(msgTokens.parameters[0]);
	client.setRealname(msgTokens.trailing);

	if(client.getNickname().empty())
	{
		sendMessage(client, NTC_NICKMISSING(this->_serverName, client.getNickname()));
		return;
	}

	client.setRegistration(true);
	sendMessage(client, RPL_WELCOME(this->_serverName, client.getNickname(), client.getUsername(), client.getClientIp()));
	sendMessage(client, RPL_YOURHOST(this->_serverName, client.getNickname()));
	sendMessage(client, RPL_CREATED(this->_serverName, client.getNickname()));
	sendMessage(client, RPL_MYINFO(this->_serverName, client.getNickname()));
}

/*
 *	isUserValid()
 *
 *	Strictly according to RFC 2812, the only mandatory rules for the username 
 *	(sent with the USER command) are:
 *	 -	Maximum length is 9 characters
 *	 -	Must not contain spaces (since it's a single parameter, spaces would 
 *		break parsing).
 *	Since the protocol is vague about other username restrictions, different 
 *	IRC servers enforce their own rules. For our own server, we have decided
 *	to keep usernames simple by following the same rules as the nickname: only
 *	ALPHANUMERIC characters, HYPHENS (-) and UNDERSCORES (_) allowed.
 *	 - No special characters like @, !, #, *, etc. that may have meanings in IRC
 *	 - No non-printable characers (ascii 1-31, 127)
 */
bool	Server::isUserValid(const std::string &username) 
{
    if (username.empty() || username.length() > 9)
        return (false);

    for (size_t i = 0; i < username.length(); i++) 
    {
        if (!isalnum(username[i]) && username[i] != '-' && username[i] != '_')
            return (false);
    }

    return (true);
}
