/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:55:20 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/21 01:55:51 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void 	Server::handleUser(Client &currentClient, const t_tokens msgTokens)
{
	if(currentClient.getAuthentication() == true && !(currentClient.getNickname().empty()))
	{
		currentClient.setUsername(msgTokens.parameters[0]);
		currentClient.setRegistration(true);
		std::cout << "\n-> username received, client is registered; your username is " << msgTokens.parameters[0] << std::endl;

		std::string rpl_welcome = "001 " + currentClient.getNickname() + " :Welcome to the Internet Relay Network, " + currentClient.getNickname() + "!\r\n";
		send(currentClient.getFd(), rpl_welcome.c_str(), rpl_welcome.size(), 0);
		std::string rpl_yourhost = "002 " + currentClient.getNickname() + " :Your host is localhost, running version 1.0\r\n";
		send(currentClient.getFd(), rpl_yourhost.c_str(), rpl_yourhost.size(), 0);
		std::string rpl_created = "003 " + currentClient.getNickname() + " :This server was created today\r\n";
		send(currentClient.getFd(), rpl_created.c_str(), rpl_created.size(), 0);
		std::string rpl_myinfo = "004 " + currentClient.getNickname() + " localhost 1.0 -availableusermodes- -availablechannelmodes-\r\n";
		send(currentClient.getFd(), rpl_myinfo.c_str(), rpl_myinfo.size(), 0);	
	}
	else
		std::cout << "client is not authenticated, I will not store the USERNAME" << std::endl;	
}