/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 22:47:55 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/19 20:03:58 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_pong(Client &client, const t_tokens msgTokens)
{
	std::string	serverName = msgTokens.parameters[0];
	
	std::cout << "[~DEBUG]: " << getCurrentDate() << " :PONG sent to client " 
			  << client.getFd() << ": " << client.getNickname() << std::endl;

	// _debugListClients();
	// _debugListChannels();
	
	_sendMessage(client, "PONG " + serverName + "\r\n");
}
