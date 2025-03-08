/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 19:02:42 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/08 23:24:10 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_privmsg(Client &client, const t_tokens msgTokens)
{
	if (!client.getRegistration())
	{
		_sendMessage(client, ERR_NOTREGISTERED(this->_serverName, client.getNickname()));
		return;
	}
	
	if (msgTokens.parameters.empty())
	{
		_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return;
	}

	if (msgTokens.trailing.empty())
	{
		_sendMessage(client, ERR_NOTEXTTOSEND(this->_serverName, client.getNickname()));
		return;
	}
	
	std::vector<std::string> targets = _splitByComma(msgTokens.parameters[0]);

	for (size_t i = 0; i < targets.size(); i++)
	{
		std::string &target = targets[i];

		if (target[0] == '#' || target[0] == '&')
		{
			// If it's a valid channel name, we broadcast the message to the channel
			if (_chanExists(target)) 
			{
				_sendToChannel(client, target, msgTokens);
				continue ;
			}
			else
			{
				_sendMessage(client, ERR_NOSUCHCHANNEL(this->_serverName, client.getNickname(), target));
				continue;
			}
		}
		// If the nickname exists, we send to user
		if (_nickExists(target))
		{
			_sendToUser(client, target, msgTokens);
			continue ;
		} 
		else
			_sendMessage(client, ERR_NOSUCHNICK(this->_serverName, client.getNickname(), target));
	}
}

void	Server::_sendToChannel(Client &client, const std::string &target, const t_tokens msgTokens)
{
	(void)client;
	std::cout << "Message should be broadcasted to full " << target << " channel" << std::endl;

	/* ********************************************************************** */
	/*                                                                        */
	/*                      ESPAI RESERVAT PER L'ESTER                        */
	/*                                                                        */
	/* ********************************************************************** */

	std::cout << msgTokens.trailing << std::endl;
}

void	Server::_sendToUser(Client &client, const std::string &target, const t_tokens msgTokens)
{
	std::map<int, Client>:: iterator	it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second.getNickname() == target)
		{
			std::string fullMessage = ":" + client.getHostMask() + " " + msgTokens.command + " " + target + " :" + msgTokens.trailing + "\r\n";
			_sendMessage(it->second, fullMessage);
			return;
		}
	}
}
