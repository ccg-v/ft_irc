/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 21:43:10 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/25 22:39:03 by erosas-c         ###   ########.fr       */
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

	std::string channelName = toLowerStr(msgTokens.parameters[0]);

	Channel *channel = _findChannelByName(channelName);

	if (!channel)
	{
		_sendMessage(client, ERR_NOSUCHCHANNEL(_serverName, client.getNickname(), channelName));
		return;
	}

	if (!channel->isMember(client.getFd()))
	{
		_sendMessage(client, ERR_NOTONCHANNEL(_serverName, client.getNickname(), channelName));
		return ;			
	}

	if (!client.isOperator(channelName))
	{
		_sendMessage(client, ERR_CHANOPRIVSNEEDED(this->_serverName, client.getNickname(), channelName));
		return ;
	}

	std::vector<std::string> nicks = splitByComma(msgTokens.parameters[1]);

	for (size_t i = 0; i < nicks.size(); i++)
	{
 		Client *kickedClient = _findClientByNick(nicks[i]);

		if (!kickedClient)
		{
			_sendMessage(client, ERR_NOSUCHNICK(this->_serverName, client.getNickname(), nicks[i]));
			continue;
		}

		if (_onChannel(*kickedClient, channelName))
		{
			// Broadcast message to channel
			std::vector<std::string>	messages;
 			std::string message = ":" + client.getHostMask() + " " + msgTokens.command + 
								  " " + channelName + " " + kickedClient->getNickname() + 
								  " " + msgTokens.trailing + "\r\n";
			messages.push_back(message);

			_broadcastToChannel(*channel, messages);

			channel->removeMember(kickedClient->getFd()); // In the channel, remove the client from the channel's members
			kickedClient->unsubscribe(channelName); 	  // In the client, remove subscription to the channel
		}
		else
		{
 			this->_sendMessage(client, ERR_USERNOTINCHANNEL(this->_serverName, nicks[i], channelName));
			 continue ;
		}
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

/* 
 *	[1]	ERR_CHANOPRIVSNEEDED should be built like this:
 *
 *	_sendMessage(client, ERR_CHANOPRIVSNEEDED(this->_serverName, client.getNickname(), channelName));
 *
 *		However, for some reason when an IRSSI client receives the message it closes
 *		the channel window unexpectedly. I overcame the issue building and sending my
 *		own message, making it clear that in the message that its not the standard 
 *		reply but a customized notice of our own.
 *
 * 	Alternative to ERR_CHANOPRIVSNEEDED
 * 		_sendMessage(client, ":" + this->_serverName + " NOTICE " + client.getNickname() + \
 * 		" " + channelName + " :You're not channel operator\r\n");
 */
