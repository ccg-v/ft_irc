/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 19:14:04 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/17 01:08:02 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::_part(Client &client, const t_tokens msgTokens)
{
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
		if (channel)
		{
			std::vector<int> members = channel->getClients();

			// Remove the client among the channel's members
			std::vector<int>::iterator clientIt = std::find(members.begin(), members.end(), client.getFd());
			if (clientIt != members.end())
				members.erase(clientIt);

			channel->setClients(members);

			std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
			" " + channelName + " " + msgTokens.trailing + "\r\n";
		
			for (size_t i = 0; i < members.size(); i++)
			{
			Client *member = _findClientByFd(channel->getClients()[i]);
		
			if (member && member->getFd() != client.getFd()) // Don't send to sender
			_sendMessage(*member, message);
			}

		}
		subscriptions.erase(it); // Erase current element
		it = next; // Continue with the next valid iterator
	}



}

/*
 *	[1]	
 *		for (subscriptions.it.begin(); subscriptions.it.end(), it++) {}
 */