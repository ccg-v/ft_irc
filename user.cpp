/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:55:20 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/23 23:02:51 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void 	Server::handleUser(Client &currentClient, const t_tokens msgTokens)
{
	if(currentClient.getAuthentication() == false || currentClient.getNickname().empty())
	{
		std::cout << "client is not authenticated, I will not store the USERNAME" << std::endl;
		return;
	}
	
	if (msgTokens.parameters.size() != 3 && msgTokens.trailing.empty())
	{
		sendMessage(currentClient, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return;
	}

	currentClient.setUsername(msgTokens.parameters[0]);
	currentClient.setRegistration(true);

	std::cout << "\n-> username received, client is registered; your username is " << msgTokens.parameters[0] << std::endl;

	std::string	message;
	message = RPL_WELCOME(this->_serverName, currentClient.getNickname(), currentClient.getUsername(), msgTokens.parameters[2]);		
	send(currentClient.getFd(), message.c_str(), message.size(), 0);

	message = RPL_YOURHOST(this->_serverName, currentClient.getNickname());
	send(currentClient.getFd(), message.c_str(), message.size(), 0);

	message = RPL_CREATED(this->_serverName, currentClient.getNickname());
	send(currentClient.getFd(), message.c_str(), message.size(), 0);

	message = RPL_MYINFO(this->_serverName, currentClient.getNickname());
	send(currentClient.getFd(), message.c_str(), message.size(), 0);
}