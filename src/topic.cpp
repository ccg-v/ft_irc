/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 23:57:21 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/20 13:51:47 by ccarrace         ###   ########.fr       */
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

	std::string channelName = msgTokens.parameters[0];

	Channel *channel = _findChannelByName(channelName);

	if (!channel)
	{
		_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), channelName));
		return;
	}

	std::map<std::string, bool> &subscriptions = client.getChannels();
	std::map<std::string, bool>::iterator it;

	it = subscriptions.find(channelName);

    if (it == subscriptions.end())
	{
		_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), channelName));
		return ;
	}

	bool isOperator = it->second;
	if (!isOperator)
	{
		// Alternative to ERR_CHANOPRIVSNEEDED (see footnote [1])
		_sendMessage(client, ":" + this->_serverName + " NOTICE " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n"); // [1]
		return ;
	}

	std::string oldTopic = channel->getTopic();
	channel->setTopic(msgTokens.trailing);
	std::string	newTopic = channel->getTopic();

	if (!oldTopic.empty() && newTopic.empty())
		std::cout << "TOPIC command has been cleared " << channel->getTopic() << std::endl;
	else
	{
		// BROADCAST message to channel		
		std::string message = RPL_TOPIC(this->_serverName, client.getNickname(), channelName, newTopic);

		for (size_t i = 0; i < channel->getClients().size(); i++)
		{
			Client *member = _findClientByFd(channel->getClients()[i]);
		
			_sendMessage(*member, message);
		}
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
 */
 