/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:20:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/23 01:31:57 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_quit(Client &client, const t_tokens msgTokens)
{
	(void)msgTokens;
	
	std::map<std::string, bool> &clientChannels = client.getChannels();
	std::map<std::string, bool>::iterator it;

	// Iterate over the client's subscription
	for (it = clientChannels.begin(); it != clientChannels.end(); ++it)
	{

		std::string channelName = it->first;

		Channel *channel = this->_findChannelByName(channelName);
	
		std::vector<std::string>	messages;
		std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
							  " " + channelName + " " + client.getNickname() + 
							  " " + msgTokens.trailing + "\r\n";
		messages.push_back(message);

		_broadcastToChannel(*channel, messages);
		
		channel->removeMember(client.getFd());
		client.unsubscribe(channelName);
	}
	
	this->_removeClient(client.getFd());
}
