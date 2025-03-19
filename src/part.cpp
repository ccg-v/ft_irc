/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 19:14:04 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/19 00:45:03 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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

		// In the channel, remove the client from the channel's members
		if (!channel->removeMember(client.getFd()))
		{
			_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), partingChannels[i]));
			continue ;			
		}
		// std::vector<int>& members = channel->getClientsByRef();
		// std::vector<int>::iterator clientIt = std::find(members.begin(), members.end(), client.getFd());
		// if (clientIt == members.end())
		// {
		// 	_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), partingChannels[i]));
		// 	continue ;
		// }
		// members.erase(clientIt);

		// In the client, remove subscription to the channel
		if(!client.unsubscribe(partingChannels[i]))
		{
			_sendMessage(client, ERR_USERNOTINCHANNEL(this->_serverName, client.getNickname(), partingChannels[i]));
			continue;
		}
		// std::map<std::string, bool>::iterator it = client.getChannels().find(partingChannels[i]);
		
		// if (it != client.getChannels().end())
		// {
		// 	client.getChannels().erase(it);
		// }
		// else
		// {
		// 	_sendMessage(client, ERR_USERNOTINCHANNEL(this->_serverName, client.getNickname(), partingChannels[i]));
		// 	continue ;
		// }		
	}
}

// void	Server::_part(Client &client, const t_tokens msgTokens)
// {
// 	if (msgTokens.parameters.size() < 1)
// 	{
// 		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
// 		return ;
// 	}

// 	std::map<std::string, bool> &subscriptions = client.getChannels();
// 	std::map<std::string, bool>::iterator it = subscriptions.begin();
	
// 	// Iterate over the client's subscription
// 	while (it != subscriptions.end())
// 	{
// 		std::map<std::string, bool>::iterator next = it; // Store the next iterator
// 		++next; // Move to the next element before erasing

// 		std::string channelName = it->first;

// 		// Find the channel object by name
// 		Channel *channel = this->_findChannelByName(channelName);
// 		if (!channel)
// 		{
// 			_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), channelName));
// 			return;
// 		}

// 		std::vector<int>& members = channel->getClientsByRef();

// 		// Remove the client among the channel's members
// 		std::vector<int>::iterator clientIt = std::find(members.begin(), members.end(), client.getFd());

// 		if (clientIt == members.end())
// 		{
// 			_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), channelName));
// 			return ;
// 		}

// 		members.erase(clientIt);
// 		// channel->setClients(members); // Store the modified list of members (client has been removed)

// 		std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
// 		" " + channelName + " " + msgTokens.trailing + "\r\n";
	
// 		for (size_t i = 0; i < members.size(); i++)
// 		{
// 			Client *member = _findClientByFd(channel->getClients()[i]);
		
// 			if (member && member->getFd() != client.getFd()) // Don't send to sender
// 			_sendMessage(*member, message);
// 		}

// 		subscriptions.erase(it); // Erase current element
// 		it = next; // Continue with the next valid iterator
// 	}
// }
