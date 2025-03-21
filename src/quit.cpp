/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:20:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/20 21:52:33 by erosas-c         ###   ########.fr       */
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
		// std::map<std::string, bool>::iterator next = it; // Store the next iterator
		// ++next; // Move to the next element before erasing

		std::string channelName = it->first; // <-- AQUI ACONSEGUEIXO EL NOM DEL CANAL AL QUAL ESTA SUBSCRIT

		// Find the channel object by name
		Channel *channel = this->_findChannelByName(channelName); // <-- DESO EL PUNTER AL CANAL
		// std::vector<int>& channelClients = channel->getClientsByRef();	// ... I ACCEDEIXO A LA LLISTA DE MEMBRES

		// // Remove the client among the channel's members
		// std::vector<int>::iterator clientIt = std::find(channelClients.begin(), channelClients.end(), client.getFd());
		// channelClients.erase(clientIt);

		/**********************************************************************/
		/*         BROADCAST QUIT TO MEMBERS OF THE CHANNELS TO LEAVE         */
		/**********************************************************************/	

		// BROADCAST message to channel		
		std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
								" " + channelName + " " + client.getNickname() + 
								" " + msgTokens.trailing + "\r\n";
		std::cout << message << std::endl;

		for (size_t i = 0; i < channel->getClients().size(); i++)
		{
			Client *member = _findClientByFd(channel->getClients()[i]);
			
			if (member && member->getFd() != client.getFd()) // Don't send to sender
				_sendMessage(*member, message);
		}
		channel->removeMember(client.getFd());
		client.unsubscribe(channelName);
	}
	this->_removeClient(client.getFd());
}
