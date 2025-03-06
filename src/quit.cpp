/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:20:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/06 23:24:42 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_quit(Client &client, const t_tokens msgTokens)
{
	(void)msgTokens;
	
	int clientFd = client.getFd();

	/* TO_DO ***************************************************************** */
	/*                                                                         */
	/*	broadcastToChannels() // Notify client quitting to its active channels */
	/*                                                                         */
	/* *********************************************************************** */
	
	_removeClient(clientFd);
}
