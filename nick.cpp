/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:52:25 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/21 01:53:08 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void 	Server::handleNick(Client &currentClient, const t_tokens msgTokens)
{
	if(currentClient.getAuthentication() == true) 
	{
		currentClient.setNickname(msgTokens.parameters[0]);
		std::cout << "\n-> nickname received; client is authenticated; now you are " << msgTokens.parameters[0] << "\n" << std::endl;
	}
	else
	{
		std::string err_passwdmismatch = msgTokens.parameters[0] + " : Password required\r\n";
		send(currentClient.getFd(), err_passwdmismatch.c_str(), err_passwdmismatch.size(), 0);
		std::cout << "client is not authenticated, I will not store the nickname" << std::endl;
	}
}