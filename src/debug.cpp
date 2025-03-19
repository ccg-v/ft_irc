/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 03:02:37 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/19 13:38:50 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::_debugListClients()
{
	std::map<int, Client>::iterator it;

	if (this->_clients.empty())
		std::cout << "[DEBUG~]:\tNo clients connected to server" << std::endl;
	else
	{
		std::cout << "[~DEBUG]:\tList of clients connected to server: " << std::endl;

		for (it = this->_clients.begin(); it != this->_clients.end(); it++)
		{
			std::cout << "\t\t- " << it->first << ": " << it->second.getNickname() << std::endl;
		}
		std::cout << std::endl;
	}
}

void Server::_debugListChannels()
{
    std::map<std::string, Channel>::iterator channelIt;

    if (this->_channels.empty())
        std::cout << "[DEBUG~]:\tNo channels on server" << std::endl;
    else
	// {
        std::cout << "[~DEBUG]:\tList of channels and their members: " << std::endl;

		// Iterate over all channels stored in server (std::map<std::string, Channel> _channels in Server)
		for (channelIt = this->_channels.begin(); channelIt != this->_channels.end(); channelIt++)
		{
			std::cout << "[~DEBUG]:\t- " << channelIt->first << std::endl;

			std::vector<int> members = channelIt->second.getClients();

			// Iterate over the members of a channel (std::vector<int> _clients in Channel)
			for (size_t i = 0; i < members.size(); i++)
			{
				int fd = members[i];
				std::string channelName = channelIt->first;

				// Check if client exists
				Client *member = _findClientByFd(fd);
				if (!member) {
					std::cerr << "Error: Client with fd " << fd << " not found among " << channelName << " members!" << std::endl;
					continue;
				}

				// Check if fd exists in _clients map
				if (_clients.find(fd) == _clients.end()) {
					std::cerr << "Error: FD " << fd << " not found in clients map!" << std::endl;
					continue;
				}

				//	// Check if member has any subscriptions to
				std::map<std::string, bool> subscriptions = member->getChannels();
				if (subscriptions.empty()) {
					// std::cerr << "Warning: Client " << member->getNickname() << " has no channel subscriptions!" << std::endl;
					continue;
				}

				std::map<std::string, bool>::iterator it = subscriptions.find(channelName);

				std::cout << "\t\t · ";
				if (it != subscriptions.end())
				{
					if (it->second == true)  
						std::cout << "+o ";
					else
						std::cout << "-o ";
				}
				std::cout << _clients[fd].getNickname() << std::endl;      
			}
		}
		std::cout << std::endl;
	// }	
}

// void	Server::_debugListChannels()
// {
// 	std::map<std::string, Channel>::iterator channelIt;
// 	if (this->_channels.empty())
// 		std::cout << "[DEBUG~]:\tNo channels on server" << std::endl;
// 	else
// 		std::cout << "[~DEBUG]:\tList of channels and their members: " << std::endl;
	
// 	for (channelIt = this->_channels.begin(); channelIt != this->_channels.end(); channelIt++)
// 	{
// 		std::cout << "[~DEBUG]:\t- " << channelIt->first << std::endl;

// 		std::vector<int> members = channelIt->second.getClients();

// 		for (size_t i = 0; i < members.size(); i++)
// 		{
// 			int fd = members[i];
// 			std::string channelName = channelIt->first;
// 			Client *member = _findClientByFd(fd);
// 			std::map<std::string, bool> subscriptions = member->getChannels();
// 			std::map<std::string, bool>::iterator it = subscriptions.find(channelName);

// 			std::cout << "\t\t · " << this->_clients[fd].getNickname();
// 			if (it != subscriptions.end())
// 			{
// 				if (it->second == true)  // Ensure the channel exists in the subscriptions map
// 					std::cout << "\t +o" << std::endl;
// 				else
// 					std::cout << "\t -o" << std::endl;
// 			}			
// 		}
// 	}
// 	std::cout << std::endl;
// }