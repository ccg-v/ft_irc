/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 23:57:21 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/26 20:16:13 by erosas-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::_topic(Client &client, const t_tokens msgTokens)
{
	if (msgTokens.parameters.size() < 1)
	{
		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return ;
	}

	std::string channelName = toLowerStr(msgTokens.parameters[0]);

	Channel *channel = _findChannelByName(channelName);

	if (!channel)
	{
		_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), channelName));
		return;
	}

	if (!channel->isMember(client.getFd()))
	{
		_sendMessage(client, ERR_NOTONCHANNEL(_serverName, client.getNickname(), channelName));
		return ;			
	}

	std::string oldTopic = channel->getTopic();
	std::string	newTopic = msgTokens.trailing;
	std::string	wasSetAt = timeToString(std::time(0));
	
	if (newTopic.empty())
	{
		if (oldTopic.empty())
			_sendMessage(client, RPL_NOTOPIC(this->_serverName, client.getNickname(), channelName));
		else
		{
			_sendMessage(client, RPL_TOPIC(this->_serverName, client.getNickname(), channelName, oldTopic));
			_sendMessage(client, RPL_TOPICWHOTIME(this->_serverName, client.getNickname(), channelName, channel->getWhoSetTopic(), wasSetAt));
		}
		return;
	}

	if (oldTopic != newTopic || !msgTokens.trailing.empty()) 
	{
		if (!channel->getTmode() || client.isOperator(channelName))
		{
			channel->setTopic(msgTokens.trailing);
			channel->setTopicAuthor(client.getNickname());

			// Broadcast messages to channel	
			std::vector<std::string>	messages;
			messages.push_back(RPL_TOPIC(this->_serverName, client.getNickname(), channelName, newTopic));
			messages.push_back(RPL_TOPICWHOTIME(this->_serverName, client.getNickname(), channelName, channel->getWhoSetTopic(), wasSetAt));

			_broadcastToChannel(*channel, messages);
		}
		else
			_sendMessage(client, ERR_CHANOPRIVSNEEDED(this->_serverName, client.getNickname(), channelName)); // [1]
	}
}

/* 
 *	[1]	ERR_CHANOPRIVSNEEDED should be built like this:
 *
 *	_sendMessage(client, ERR_CHANOPRIVSNEEDED(this->_serverName, client.getNickname(), channelName));
 *
 *		However, for some reason when an IRSSI client receives the message it closes
 *		the channel window unexpectedly. I overcame the issue building and sending my
 *		own message, making it clear that in the message that its not the standard 
 *		reply but a customized notice of our own.
 *		
 *		Alternative to ERR_CHANOPRIVSNEEDED:
 
 *	_sendMessage(client, ":" + this->_serverName + " NOTICE " + client.getNickname() + " " + \
 *   channelName + " :You're not channel operator\r\n");
 *
 */
