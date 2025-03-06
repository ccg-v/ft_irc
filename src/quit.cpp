/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:20:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/04 19:36:44 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_quit(Client &client, const t_tokens msgTokens)
{
	(void)msgTokens;
	
	int clientFd = client.getFd();
	std::cerr << "[~DEBUG]: Client " << clientFd << " quit. Closing connection." << std::endl;

	/* HEM DE COMUNICAR QUE EL CLIENT ES DESCONNECTA A TOTS ELS CANALS ON ESTA 
	CONNECTAT ACTUALMENT I A TOTES LES CONVERSES PRIVADES*/

	close(clientFd);
	_clients.erase(clientFd); // Remove from clients container

	// Find and remove client from _pollFds
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == clientFd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
}
