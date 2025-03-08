/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 21:43:10 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/09 00:37:47 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void	Server::_kick(Client &client, const t_tokens msgTokens)
{
	if (msgTokens.parameters.size() < 2)
	{
		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return ;
	}

	std::string channel = parameters[0];
	std::vector<std::string> nicks = _splitByComma(msgTokens.parameters[1]);

	if (!_chanExists(channel))
	{
		this->_sendMessage(client, ERR_NOSUCHCHANNEL(this->_serverName, client.getNickname(), channel));
		return ;
	}

	for (int i = 0; i < nicks.size(); i++)
	{
		if (_isMember(nick[i]))	// comprobar si _isMember
		{
			// guardamos la lista de clientes en el canal, o la lista de canales en el cliente
		}
		else
		{
			this->_sendMessage(client, ERR_USERNOTINCHANNEL(this->_serverName, nick[i], channel));
		}
	}
}	

bool	Server::_isMember(Channel &channel, Client &client)
{
	// std::vector<int>::iterator it;

	// for (it = channel->_clients.begin(); it != channel->_clients.end(); it++)
	// {
	// 	if (*it == client.getFd())
	// 		return (true);
	// }
	// return (false);
	return (std::find(channel._clients.begin(), channel._clients.end(), client.getFd()) != channel._clients.end());
}