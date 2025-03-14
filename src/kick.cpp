/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 21:43:10 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/14 23:04:58 by ccarrace         ###   ########.fr       */
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
	if (!channel)
	{
		_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), channelName));
		return;
	}

	/* COMPROVAR QUE QUI EXECUTA EL KICK TÉ EL RANG D'OPERADOR DEL CANAL????? */
	std::map<std::string, bool> &subscriptions = client.getChannels();
	std::map<std::string, bool>::iterator it;

	// for (it = subscriptions.begin(); it != subscriptions.end(); it++)
	// {
	// 	if (it->first == channelName)
	// 		std::cout << client.getNickname() << " is a channel operator" << std::endl;
	// 	else
	// 		std::cout << " is NOT an operator!!" << std::endl;
	// }

	it = subscriptions.find(channelName);
    if (it != subscriptions.end()) {
		if (it->second == true)
		{
			std::cout << client.getNickname() << " is an operator in " << channelName<< std::endl;
		}
		else
		{
        	std::cout << client.getNickname() << " is NOT an operator in " << channelName<< std::endl;
			return ;
		}
    }

	std::vector<std::string> nicks = splitByComma(msgTokens.parameters[1]);

	for (size_t i = 0; i < nicks.size(); i++)
	{
		Client * kickedClient = _findClientByNick(nicks[i]);

		if (kickedClient && _isClientInChannel(*channel, *kickedClient))
		{
			// std::string message = INF_KICKEDFROMCHANNEL(this->_serverName, msgTokens.command, channel->getName(), member->getNickname());
			std::string message = kickedClient->getNickname() + " has been kicked from " + channel->getName() + "\r\n";
			this->_sendToChannel(client, *channel, msgTokens);
			channel->removeMember(kickedClient->getFd());
			kickedClient->unsubscribe(channelName);
		}
		else
		{
std::cout << "_kick(): " << kickedClient->getNickname() << " is not a member of " << channel->getName() << std::endl;
 			this->_sendMessage(client, ERR_USERNOTINCHANNEL(this->_serverName, nicks[i], channelName));
		}
		continue ;
	}
}

Channel *Server::_findChannelByName(const std::string &channelName)
{
    std::map<std::string, Channel>::iterator it = this->_channels.find(channelName);
    
    if (it == _channels.end())
        return (NULL);
    
    return (&it->second);
}

Client *Server::_findClientByNick(const std::string &nickname)
{
	std::map<int, Client>::iterator it;

	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (it->second.getNickname() == nickname)
			return (&it->second);
	}
	return (NULL);
}

Client *Server::_findClientByFd(const int fd)
{
	std::map<int, Client>::iterator it;

	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (it->second.getFd() == fd)
			return (&it->second);
	}
	return (NULL);
}

bool Server::_isClientInChannel(Channel &channel, Client &client)
{
	std::vector<int>::iterator it;

	for (it = channel.getClients().begin(); it != channel.getClients().end(); it++)
	{
		if (*it == client.getFd())
			return (true);
	}
	return (false);
}
