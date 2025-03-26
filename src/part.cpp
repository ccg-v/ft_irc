/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 19:14:04 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/25 20:09:58 by erosas-c         ###   ########.fr       */
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

	std::vector<std::string> partingChannels = splitByComma(toLowerStr(msgTokens.parameters[0]));

	for (size_t i = 0; i < partingChannels.size(); ++i)
	{
		// Find the channel object by name
		Channel *channel = this->_findChannelByName(partingChannels[i]);
		if (!channel)
		{
			_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), partingChannels[i]));
			continue;
		}

		// Check if the client is member of the channel and he channel is among the client's subscriptions
		if (!channel->isMember(client.getFd()) || !client.isSubscriber(partingChannels[i]))
		{
			_sendMessage(client, ERR_NOTONCHANNEL(_serverName, client.getNickname(), partingChannels[i]));
			continue ;			
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
