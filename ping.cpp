/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 22:32:15 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/02 22:37:27 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_ping(Client &client, const t_tokens msgTokens)
{
	std::cout << "[~DEBUG]: " << getCurrentDate() << " :PING received from client " << client.getFd() << std::endl;
	
	this->_pong(client, msgTokens);
}