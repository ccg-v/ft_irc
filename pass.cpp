/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:43:33 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/27 20:28:13 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

/* See RFC 2812, Section 3.1.1 */

void 	Server::handlePass(Client &client, const t_tokens msgTokens)
{
	if (msgTokens.parameters.empty() || msgTokens.parameters[0] != this->_password)
	{
		sendMessage(client, ERR_PASSWDMISMATCH(this->_serverName, client.getNickname()));

		// Close client's socket and remove client from tracking containers
		this->removeClient(client.getFd());

		return;
	}

	if (client.getAuthentication() == true)
	{
		sendMessage(client, ERR_ALREADYREGISTERED(this->_serverName));
		return;
	}

	client.setAuthentication(true);
}
