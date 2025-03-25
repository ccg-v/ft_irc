/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 22:32:15 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/17 23:56:12 by erosas-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_ping(Client &client, const t_tokens msgTokens)
{
	if (msgTokens.parameters.size() == 0)
	{
		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return ;
	}
	std::cout << "[~DEBUG]: " << getCurrentDate() << " :PING received from client " 
			  << client.getFd() << ": " << client.getNickname() << std::endl;
	
	this->_pong(client, msgTokens);
}
