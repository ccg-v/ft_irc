/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 23:42:53 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/13 01:05:46 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

#define BACKLOG 5        // Max number of pending connections queue will hold
#define BUFFER_SIZE 512  // Max buffer size for recv() [1]

class	Server
{
	public:

		Server(const std::string & port, const std::string & password);	// Parameterized constructor
		~Server();														// Default destructor

		void	startPoll();

	private:

		std::string					_port;
		std::string 				_password;
		int							_serverSocket;
    	std::vector<struct pollfd> 	_pollFds;  		// Polling sockets
    	std::map<int, Client> 		_clients;  		// Map of descriptors (key) and clients

		Server(const Server & source);				// Copy constructor [1]
		Server & operator= (const Server & source);	// Copy assignment operator [1]

		/* --- Rest of private methods  ------------------------------------- */

		void	acceptClient();
		void	receiveData(int i);
		void	closeSockets();
}

/*
 *	[1]	An IRC server should not allow copies. A server typically manages sockets,
 *		file descriptors, and client connections, which are unique resources that
 *		should not be duplicated. Making private both the copy constructor and the
 *		copy assignment operator prevents server copying. No need to define them.
 */