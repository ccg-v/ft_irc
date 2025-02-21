/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 01:43:33 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/21 01:46:29 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

void 	Server::handlePass(Client &currentClient, const t_tokens msgTokens)
{
	if (msgTokens.parameters[0] == this->_password)
		currentClient.setAuthentication(true);
	else
	{
		std::string err_passwdmismatch = "* : Password required\r\n";
		send(currentClient.getFd(), err_passwdmismatch.c_str(), err_passwdmismatch.size(), 0);
	}	
}