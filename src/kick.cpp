/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 21:43:10 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/20 21:51:51 by erosas-c         ###   ########.fr       */
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
	// If channel doesn't exist, throw 403: ERR_NOSUCHCHANNEL
	if (!channel)
	{
		_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), channelName));
		return;
	}

	std::map<std::string, bool> &subscriptions = client.getChannels();
	std::map<std::string, bool>::iterator it;

	it = subscriptions.find(channelName);
	// if channel exists but kicker client is not a member, throw 442: ERR_NOTONCHANNEL
    if (it == subscriptions.end())
	{
		_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), channelName));
		return ;
	}

	bool isOperator = it->second;
	if (!isOperator)
	{
		// std::cout << client.getNickname() << " is NOT operator in " << channelName << "(482: ERR_CHANOPRIVSNEEDED)" << std::endl;
		_sendMessage(client, ERR_CHANOPRIVSNEEDED(this->_serverName, client.getNickname(), channelName));
		/* ********************************************************************************************************** */
		/* HAIG DE CONSTRUIR UNA NOTIFICACIO PERQUE QUAN IRSSi REP "ERR_CHANOPRIVSNEEDED" TANCA LA FINESTRA DEL CANAL */
		/* ********************************************************************************************************** */
		//_sendMessage(client, ":" + this->_serverName + " NOTICE " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n");
		return ;
	}

	std::vector<std::string> nicks = splitByComma(msgTokens.parameters[1]);

	for (size_t i = 0; i < nicks.size(); i++)
	{
 		Client *kickedClient = _findClientByNick(nicks[i]);

		if (!kickedClient)
		{
			_sendMessage(client, ERR_ERRONEUSNICKNAME(this->_serverName, nicks[i]));
			continue;
		}
		// if (_isClientInChannel(*channel, *kickedClient))
		if (_onChannel(*kickedClient, channelName))
		{
			// std::string message = INF_KICKEDFROMCHANNEL(this->_serverName, msgTokens.command, channel->getName(), member->getNickname());
			// std::string message = kickedClient->getNickname() + " has been kicked from " + channel->getName() + "\r\n";
			// std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
			// 					  " " + channelName + " " + kickedClient->getNickname() +
			// 					  " :" + client.getNickname();
			// std::cout << message << std::endl;
			
// this->_sendToChannel(client, *channel, msgTokens);
 			std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
								  " " + channelName + " " + kickedClient->getNickname() + 
								  " " + msgTokens.trailing + "\r\n";
			std::cout << message << std::endl;
			// this->_sendToChannel(client, *channel, msgTokens);

			// BROADCAST message to channel
			for (size_t i = 0; i < channel->getClients().size(); i++)
			{
				Client *member = _findClientByFd(channel->getClients()[i]);
				
				if (member && member->getFd() != client.getFd()) // Don't send to sender
					_sendMessage(*member, message);
			}

			channel->removeMember(kickedClient->getFd());
			kickedClient->unsubscribe(channelName);
		}
		else
		{
// std::cout << "_kick(): " << kickedClient->getNickname() << " is not a member of " << channel->getName() << std::endl;
 			this->_sendMessage(client, ERR_USERNOTINCHANNEL(this->_serverName, nicks[i], channelName));
			 continue ;
		}
		// continue ;
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

// bool Server::_isClientInChannel(Channel &channel, Client &client)
// {
// 	std::vector<int>::iterator it;

// 	for (it = channel.getClients().begin(); it != channel.getClients().end(); it++)
// 	{
// 		if (*it == client.getFd())
// 			return (true);
// 	}
// 	return (false);
// }
