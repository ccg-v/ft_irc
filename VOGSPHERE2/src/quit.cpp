/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:20:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/24 23:04:05 by erosas-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_quit(Client &client, const t_tokens msgTokens)
{
	(void)msgTokens;
	
	std::vector<std::string> channelsToLeave;
	std::map<std::string, bool> &clientChannels = client.getChannels();

	// Collect channel names first
	for (std::map<std::string, bool>::iterator it = clientChannels.begin(); it != clientChannels.end(); ++it)
		channelsToLeave.push_back(it->first);

	// Iterate over stored channel names to avoid iterator invalidation
	for (size_t i = 0; i < channelsToLeave.size(); i++)
	{
		std::string channelName = channelsToLeave[i];
		Channel *channel = this->_findChannelByName(channelName);
		if (!channel)
			continue;  // Safety check if channel is null
			
		std::vector<std::string>	messages;
		std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
							  " " + channelName + " " + client.getNickname() + 
							  " " + msgTokens.trailing + "\r\n";
		messages.push_back(message);
		
		_broadcastToChannel(*channel, messages);

		// Remove client from channel and unsubscribe
		channel->removeMember(client.getFd());
		// client.unsubscribe(channelName);
	}
	this->_removeClient(client.getFd());
}
