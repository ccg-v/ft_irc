/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:48:59 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/21 01:50:51 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void 	Server::handleCap(Client &currentClient, const t_tokens msgTokens)
{
	if (msgTokens.command == "CAP" && msgTokens.parameters[0] != "END")
	{
		std::string msg = ":localhost CAP * LS :None\r\n";
		send(currentClient.getFd(), msg.c_str(), msg.size(), 0);
	}
}