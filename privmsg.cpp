/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 19:02:42 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/01 23:55:08 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void	Server::_privmsg(Client &client, const t_tokens msgTokens)
{
	if (msgTokens.parameters.size() < 1)
	{
		sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msgTokens.command));
		return;
	}
	std::vector<std::string>	targets;

	targets = _splitByComma(msgTokens.parameters[0]);
	for (size_t i = 0; i < targets.size(); i++)
	{
		if (targets[i][0] == '#' || targets[i][0] == '&')
		{
			if (_validChannelName(targets[i]))
				std::cout << "Message broadcasted to full " << targets[i] << " channel" << std::endl;
			else
			{
				
			}
		}
		else
		{
			if (isNickValid(targets[i]))
				std::cout << "Message sent to " << targets[i] << " client" << std::endl;
			else
			{
				sendMessage(client, ERR_ERRONEUSNICKNAME(this->_serverName, targets[i]));
				return ;
			}
		}
	}

}