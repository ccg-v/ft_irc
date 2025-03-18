/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 19:14:04 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/18 01:10:21 by ccarrace         ###   ########.fr       */
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

	std::map<std::string, bool> &subscriptions = client.getChannels();
	std::map<std::string, bool>::iterator it = subscriptions.begin();
	
	// Iterate over the client's subscription
	while (it != subscriptions.end())
	{
		std::map<std::string, bool>::iterator next = it; // Store the next iterator
		++next; // Move to the next element before erasing

		std::string channelName = it->first;

		// Find the channel object by name
		Channel *channel = this->_findChannelByName(channelName);
		if (!channel)
		{
			_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), channelName));
			return;
		}

		std::vector<int> members = channel->getClients();

		// Remove the client among the channel's members
		std::vector<int>::iterator clientIt = std::find(members.begin(), members.end(), client.getFd());

		if (clientIt == members.end())
		{
			_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), channelName));
			return ;
		}

		members.erase(clientIt);
		channel->setClients(members); // Store the modified list of members (client has been removed)

		std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
		" " + channelName + " " + msgTokens.trailing + "\r\n";
	
		for (size_t i = 0; i < members.size(); i++)
		{
		Client *member = _findClientByFd(channel->getClients()[i]);
	
		if (member && member->getFd() != client.getFd()) // Don't send to sender
		_sendMessage(*member, message);
		}

		subscriptions.erase(it); // Erase current element
		it = next; // Continue with the next valid iterator
	}
}
