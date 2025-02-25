/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:55:20 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/25 18:52:22 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void 	Server::handleUser(Client &client, const t_tokens msgTokens)
{
	if(client.getAuthentication() == false)
	{
		sendMessage(client, ERR_PASSWDMISMATCH(this->_serverName, client.getNickname()));
		return;
	}

	if(client.getRegistration() == true)
	{
		sendMessage(client, ERR_ALREADYREGISTERED(this->_serverName));
		return;
	}

	if (msgTokens.parameters.size() != 3 || msgTokens.trailing.empty())
	{
		sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
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

	// std::cout << "\n-> username received, client is registered; your username is " << msgTokens.parameters[0] << std::endl;

	sendMessage(client, RPL_WELCOME(this->_serverName, client.getNickname(), client.getUsername(), client.getClientIp()));
	sendMessage(client, RPL_YOURHOST(this->_serverName, client.getNickname()));
	sendMessage(client, RPL_CREATED(this->_serverName, client.getNickname()));
	sendMessage(client, RPL_MYINFO(this->_serverName, client.getNickname()));

}