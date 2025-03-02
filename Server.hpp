/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 23:42:53 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/02 17:21:10 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <cstring>      // memset(), strcpy()
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
#include <arpa/inet.h>  // inet_ntoa
#include "Client.hpp"
#include "Channel.hpp"
#include "Errors.hpp"
#include "Replies.hpp"

#define BUFFER_SIZE 512  // Max buffer size for recv() [1]
#define BACKLOG 5        // Max number of pending connections queue will hold

typedef struct s_tokens
{
	std::string					command;
	std::vector<std::string>	parameters;
	std::string					trailing;
} 	t_tokens;

class	Client;
class	Channel;

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

		std::string						_port;
		std::string 					_password;
		std::string						_serverName;
		std::string						_creationTime;
		int								_serverSocket;
    	std::vector<struct pollfd> 		_pollFds;  		// Polling sockets
    	std::map<int, Client> 			_clients;  		// Map of descriptors (key) and clients
		std::map<std::string, Channel>	_channels; // key in the map is a channel's name

		std::map<std::string, void (Server::*)(Client&, t_tokens)> _commandMap; // [2]
		
		/* --- Private Coplien's functions ---------------------------------- */

		Server(const Server &source);				// Copy constructor [1]
		Server &operator=(const Server &source);	// Copy assignment operator [1]

		void						acceptClient();
		void						receiveRawData(Client &currentClient, size_t &i);
		std::vector<std::string>	splitBuffer(std::string & buffer);
		void						processMessage(Client &currentClient, std::string message);
		t_tokens					tokenizeMsg(const std::string  &message);
		void						sendMessage(Client &client, const std::string &message);
		void						removeClient(int clientFd);
		void						closeSockets();
		
		void 		handleCap(Client &client, const t_tokens msgTokens);
		void 		handlePass(Client &client, const t_tokens msgTokens);
		void 		handleNick(Client &client, const t_tokens msgTokens);
		void 		handleUser(Client &client, const t_tokens msgTokens);
		void 		_join(Client &client, const t_tokens msgTokens);
		void 		_ping(Client &client, const t_tokens msgTokens);
		void 		_pong(Client &client, const t_tokens msgTokens);
		void 		_privmsg(Client &client, const t_tokens msgTokens);		
        // void _kick();
        // void _invite();
        // void _topic(int& i, std::vector<std::string> &args);
        // void _mode();

		bool	_isNickValid(const std::string &nick);
		bool	_isNickTaken(const std::string &nick);
		bool 	_isUserValid(const std::string &username);

		void	_sendToChannel(Client &client, const std::string &target, const t_tokens msgTokens);
		void	_sendToUser(Client &client, const std::string &target, const t_tokens msgTokens);

		//* --- Join ---------------------------------- */
		bool						_chanExists(std::string &);
		bool 						_validChannelName(std::string &name);
		std::vector<std::string>	_splitByComma(const std::string &str);
};

#endif

/*
 *	[1]	An IRC server should not allow copies. A server typically manages sockets,
 *		file descriptors, and client connections, which are unique resources that
 *		should not be duplicated. Making private both the copy constructor and the
 *		copy assignment operator prevents server copying. No need to define them.
 */

/*
 *	[2]	We define a map where:
 *		- The key
 *
 *				(std::string)
 *
 *			is the command name (e.g., "JOIN", "PRIVMSG", etc.).
 *
 *		- The value
 *
 *				(void (Server::*)(const std::vector<std::string>&, int))
 *
 *			is a pointer to a member function inside Server that handles the command.
 *			Here we are using the raw function pointer syntax (not a typedef or a
 *			regular function definition), where parameter names are innecessary.
 *
 *			The parentheses are strictly necessary in this case because of C++'s 
 *			parsing rules for function pointer types.
 *			void (Server::*)(...) means "a pointer to a member function of Server 
 *			returning void"
 *			Without parentheses,
 *
 *				void Server::*commandHandler(const std::vector<std::string>&, int);
 *
 *			C++ interprets this as a function called commandHandler that returns a
 *			pointer to a member function (void Server::*), which doesn't make sense 
 *			in this context
 */
