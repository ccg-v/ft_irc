/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:20:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/19 01:12:03 by ccarrace         ###   ########.fr       */
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
		std::map<std::string, bool>::iterator next = it; // Store the next iterator
		++next; // Move to the next element before erasing

		std::string channelName = it->first; // <-- AQUI ACONSEGUEIXO EL NOM DEL CANAL AL QUAL ESTA SUBSCRIT

		// Find the channel object by name
		// Channel *channel = this->_findChannelByName(channelName); // <-- INSTANCIO EL CANAL...
		// std::vector<int>& channelClients = channel->getClientsByRef();	// ... I ACCEDEIXO A LA LLISTA DE MEMBRES

		// // Remove the client among the channel's members
		// std::vector<int>::iterator clientIt = std::find(channelClients.begin(), channelClients.end(), client.getFd());
		// channelClients.erase(clientIt);

		// channel->removeMember(client.getFd());
		// client.unsubscribe(channelName);
		std::cout << "Client to remove from " << channelName << ": " << client.getNickname() << "(" << client.getFd() << ")" << std::endl;
		std::cout << "Subscription to remove from " << client.getNickname() << ": " << channelName << std::endl;
	}

	std::cout << "=============== QUIT ===============" << std::endl;
	_debugListChannels();
	std::cout << "====================================" << std::endl;
}
