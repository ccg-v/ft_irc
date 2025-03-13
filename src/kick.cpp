/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 21:43:10 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/13 18:18:58 by ccarrace         ###   ########.fr       */
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

	std::string channelName = msgTokens.parameters[0];

	Channel *channel = _findChannelByName(channelName);
std::cout << "Channel found's name is " << channel->getName() << std::endl; 
	if (!channel)
	{
		_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), channelName));
		return ;
	}

	/* COMPROVAR QUE QUI EXECUTA EL KICK TÉ EL RANG D'OPERADOR DEL CANAL????? */
	bool	isOperator = client.getSubscriptions()[channel];

std::cout << client.getNickname() << " isOperator is " << isOperator << std::endl;
	if (!isOperator)
	{
		std::cout << "kicker is NOT an operator!!" << std::endl;
		return ;
	}

	std::vector<std::string> nicks = splitByComma(msgTokens.parameters[1]);

	for (size_t i = 0; i < nicks.size(); i++)
	{
		Client *kickedClient = _findClientByNick(nicks[i]);
		
		if (kickedClient && _isClientInChannel(*channel, *kickedClient))
		{
			// std::string message = INF_KICKEDFROMCHANNEL(this->_serverName, msgTokens.command, channel->getName(), client.getNickname());
			std::string message = nicks[i] + " has been kicked from " + channel->getName() + "\r\n" ;
			this->_sendToChannel(*channel, message);
			channel->removeMember(kickedClient);
			kickedClient->unsubscribe(*channel);
		}
		else
		{
			this->_sendMessage(client, ERR_USERNOTINCHANNEL(this->_serverName, nicks[i], channelName));
		}
		continue ;
	}
}

Channel *Server::_findChannelByName(const std::string &channelName)
{
    // Step 1: Find the channel
    std::map<std::string, Channel>::iterator it = this->_channels.find(channelName);
    
    if (it == _channels.end())
        return (NULL);
    
    return (&it->second);
}

Client *Server::_findClientByNick(const std::string &nickname)
{
	// Step 2: Find the client
	std::map<int, Client>::iterator it;

	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (it->second.getNickname() == nickname)
			return (&it->second);
	}
	return (NULL);
}

bool Server::_isClientInChannel(Channel &channel, Client &client)
{
    // Step 3: Check if the client is in the channel
	std::set<Client*>::iterator it;

	for (it = channel.getMembers().begin(); it != channel.getMembers().end(); it++)
	{
		if (*it == &client)
			return (true);
	}
	return (false);
}
