/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 19:14:04 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/23 01:44:46 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

void	Server::_part(Client &client, const t_tokens msgTokens)
{
	if (msgTokens.parameters.size() < 1)
	{
		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return ;
	}

	std::vector<std::string> partingChannels = splitByComma(msgTokens.parameters[0]);

	for (size_t i = 0; i < partingChannels.size(); ++i)
	{
		// Find the channel object by name
		Channel *channel = this->_findChannelByName(partingChannels[i]);
		if (!channel)
		{
			_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), partingChannels[i]));
			continue;
		}

		// Check if the client is member of the channel
		if (!channel->isMember(client.getFd()))
		{
			_sendMessage(client, ERR_NOTONCHANNEL(_serverName, client.getNickname(), partingChannels[i]));
			continue ;			
		}

		// Check if the channel is among the client's subscriptions
		if(!client.isSubscriber(partingChannels[i]))
		{
			_sendMessage(client, ERR_USERNOTINCHANNEL(_serverName, client.getNickname(), partingChannels[i]));
			continue;
		}

		// Broadcast message to channel
		std::vector<std::string>	messages;
		std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
							  " " + partingChannels[i] + " " + msgTokens.trailing + "\r\n";
		messages.push_back(message);

		_broadcastToChannel(*channel, messages);
				
		channel->removeMember(client.getFd());	// In the channel, remove the client from the channel's members
		client.unsubscribe(partingChannels[i]);	// In the client, remove subscription to the channel

	}
}

// void	Server::_part(Client &client, const t_tokens msgTokens)
// {
// 	if (msgTokens.parameters.size() < 1)
// 	{
// 		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
// 		return ;
// 	}

// 	std::vector<std::string> partingChannels = splitByComma(msgTokens.parameters[0]);

// 	for (size_t i = 0; i < partingChannels.size(); ++i)
// 	{
// 		// Find the channel object by name
// 		Channel *channel = this->_findChannelByName(partingChannels[i]);
// 		if (!channel)
// 		{
// 			_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), partingChannels[i]));
// 			continue;
// 		}

// 		// In the channel, remove the client from the channel's members
// 		if (!channel->removeMember(client.getFd()))
// 		{
// 			_sendMessage(client, ERR_NOTONCHANNEL(_serverName, client.getNickname(), partingChannels[i]));
// 			continue ;			
// 		}

// 		// In the client, remove subscription to the channel
// 		if(!client.unsubscribe(partingChannels[i]))
// 		{
// 			_sendMessage(client, ERR_USERNOTINCHANNEL(_serverName, client.getNickname(), partingChannels[i]));
// 			continue;
// 		}

// 		// Broadcast message to channel
// 		std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
// 							  " " + partingChannels[i] + " " + msgTokens.trailing + "\r\n";

// 		_broadcastToChannel(*channel, message);

// 	}
// }


