/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 19:02:42 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/19 00:17:26 by ccarrace         ###   ########.fr       */
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
	
	std::vector<std::string> targets = splitByComma(msgTokens.parameters[0]);

	for (size_t i = 0; i < targets.size(); i++)
	{
		std::string &target = targets[i];

		if (target[0] == '#' || target[0] == '&')
		{
			// If it's a valid channel name, we search if channel exists
			Channel *channel = _findChannelByName(target);
			if (channel) 
			{
				if (_onChannel(client, channel->getName()))
					_sendToChannel(client, *channel, msgTokens);
				else
					_sendMessage(client, ERR_USERNOTINCHANNEL(this->_serverName, client.getNickname(), channel->getName()));
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

void	Server::_sendToChannel(Client &client, Channel &channel, const t_tokens msgTokens)
{
	std::string fullMessage = ":" + client.getHostMask() + " " + msgTokens.command \
		+ " " + channel.getName() + " :" + msgTokens.trailing + "\r\n";

	for (size_t i = 0; i < channel.getClients().size(); i++)
	{
		Client *member = _findClientByFd(channel.getClients()[i]);
		
		if (member && member->getFd() != client.getFd()) // Don't send to sender
			_sendMessage(*member, fullMessage);
	}
}


// void	Server::_sendToChannel(Channel &channel, const std::string &message)
// {
// 	std::cout << "Message should be broadcasted to full " << channel.getName() << " channel" << std::endl;

// 	std::vector<int>::iterator it;
	
// 	for (size_t i = 0; i < channel.getClients().size(); i++)
// 	{
// 		Client *member = _findClientByFd(channel.getClients()[i]);
		
// 		_sendMessage(*member, message);
// 	}
// }

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
