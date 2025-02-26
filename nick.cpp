/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:52:25 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/25 21:35:23 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void 	Server::handleNick(Client &client, const t_tokens msgTokens)
{
	// Password not given, authentication pending
	if(client.getAuthentication() == false) 
	{
		sendMessage(client, ERR_PASSWDMISMATCH(this->_serverName, client.getNickname()));
		return;
	}

	// Check if a nickname has been sent (not the command alone)
	if (msgTokens.parameters.empty())
	{
		sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return;
	}

	// Check if client had already a nickname and replace it with the new one
	if(!client.getNickname().empty())
	{
		// sendMessage(client, NTC_NICKCHANGED(this->_serverName, msgTokens.parameters[0]));
		std::string oldNick = client.getNickname();
		std::string	newNick = msgTokens.parameters[0];
		
		std::string message = ":" + oldNick + "!" + client.getUsername() + "@" + client.getClientIp() \
			+ " NICK :" + newNick + "\r\n";		
		sendMessage(client, message);
		// std::cout << "Sending: " << message << std::endl;
		// send(client.getFd(), message.c_str(), message.size(), 0);
		client.setNickname(msgTokens.parameters[0]);
		return;
	}

	client.setNickname(msgTokens.parameters[0]);

	// Notice that registration is not complete, username still missing
	if(client.getUsername().empty())
	{
		sendMessage(client, NTC_USERMISSING(this->_serverName, client.getUsername()));
		return;
	}
	
	client.setRegistration(true);

	sendMessage(client, RPL_WELCOME(this->_serverName, client.getNickname(), client.getUsername(), client.getClientIp()));
	sendMessage(client, RPL_YOURHOST(this->_serverName, client.getNickname()));
	sendMessage(client, RPL_CREATED(this->_serverName, client.getNickname()));
	sendMessage(client, RPL_MYINFO(this->_serverName, client.getNickname()));

}
