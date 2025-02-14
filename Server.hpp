/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 23:42:53 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/14 22:43:07 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <cstring>      // memset()
#include <cstdlib>      // exit()
#include <sys/socket.h>	// socket(), bind(), listen(), accept()
#include <netdb.h>      // getaddrinfo(), freeaddrinfo()
#include <unistd.h>     // close()
#include <fcntl.h>      // fcntl()
#include <poll.h>       // poll()
#include <vector>		// std::vector
#include <map>			// std::map
#include <cstring>  	// strncmp()
#include <sstream>		// std::istringstream
#include "Client.hpp"

#define BUFFER_SIZE 512  // Max buffer size for recv() [1]
#define BACKLOG 5        // Max number of pending connections queue will hold

class	Client;

class	Server
{
	public:

		/* --- Public Coplien's functions ----------------------------------- */

		Server(const std::string & port, const std::string & password);	// Parameterized constructor
		~Server();														// Default destructor

		/* --- Rest of public methods  -------------------------------------- */

		void						startPoll();

	private:

		/* --- Private attributes ------------------------------------------- */

		std::string					_port;
		std::string 				_password;
		int							_serverSocket;
    	std::vector<struct pollfd> 	_pollFds;  		// Polling sockets
    	std::map<int, Client> 		_clients;  		// Map of descriptors (key) and pointers to clients

		/* --- Private Coplien's functions ---------------------------------- */

		Server(const Server & source);				// Copy constructor [1]
		Server & operator=(const Server & source);	// Copy assignment operator [1]

		/* --- Rest of private methods  ------------------------------------- */

		void						acceptClient();
		void						receiveRawData(size_t & i);
		// int							receiveRawData(int i);
		// int							receiveRawData(int &i);
		std::vector<std::string>	splitBuffer(std::string & buffer);
		void						splitMessage(int i, std::vector<std::string> fullMessages);
		void						closeSockets();
};

#endif

/*
 *	[1]	An IRC server should not allow copies. A server typically manages sockets,
 *		file descriptors, and client connections, which are unique resources that
 *		should not be duplicated. Making private both the copy constructor and the
 *		copy assignment operator prevents server copying. No need to define them.
 */