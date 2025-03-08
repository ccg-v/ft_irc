/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 23:42:53 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/09 20:28:21 by ccarrace         ###   ########.fr       */
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
#include <csignal>		// signal()
#include "Client.hpp"
#include "Channel.hpp"
#include "Replies.hpp"
#include <typeinfo> // TEMP to print typeid of getChannels ft

#define BUFFER_SIZE 512  // Max buffer size for recv() [1]
#define BACKLOG 5        // Max number of pending connections queue will hold
//#define MAXCLIENTS 4     // Max number of clients that can join a channel
// NO TE SENTIT PQ AIXO ES FA AMB el MODE 'l'
#define MAXCHAN 3        // Max number of channels a client can join

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

		void						_acceptClient();
		void						_receiveRawData(Client &currentClient, size_t &i);
		std::vector<std::string>	_splitBuffer(std::string & buffer);
		void						_processMessage(Client &currentClient, std::string message);
		t_tokens					_tokenizeMsg(const std::string  &message);
		void						_sendMessage(Client &client, const std::string &message);
		void						_removeClient(int clientFd);
		void						_removeAllClients();
		void						_closeSockets();
		
		/* --- Connection operations ---------------------------------------- */
		void 		_cap(Client &client, const t_tokens msgTokens);
		void 		_pass(Client &client, const t_tokens msgTokens);
		void 		_nick(Client &client, const t_tokens msgTokens);
		void 		_user(Client &client, const t_tokens msgTokens);

		/* --- Command handlers --------------------------------------------- */
		void 		_join(Client &client, const t_tokens msgTokens);
		void 		_ping(Client &client, const t_tokens msgTokens);
		void 		_pong(Client &client, const t_tokens msgTokens);
		void 		_privmsg(Client &client, const t_tokens msgTokens);		
        void		_kick(Client &client, const t_tokens msgTokens);
        // void _invite();
        // void _topic(int& i, std::vector<std::string> &args);
        void		_mode(Client &client, const t_tokens msgTokens);
		void		_quit(Client &client, const t_tokens msgTokens);

		/* --- Nick --------------------------------------------------------- */
		bool		_isNickValid(const std::string &nick);
		bool		_nickExists(const std::string &nick);
		bool 		_isUserValid(const std::string &username);

		/* --- Privmsg ------------------------------------------------------ */	
		void		_sendToChannel(Channel &channel, const std::string &message);
		void		_sendToUser(Client &client, const std::string &target, const t_tokens msgTokens);
		Client		*_findClientByFd(const int fd);

		/* --- Join --------------------------------------------------------- */
		bool						_chanExists(const std::string &);
		bool 						_validChannelName(std::string &name);
		std::vector<std::string>	_splitByComma(const std::string &str);

		/* --- Kick --------------------------------------------------------- */
		Channel 	*_findChannelByName(const std::string &channelName);
		Client 		*_findClientByNick(const std::string &nickname);
		bool		_isClientInChannel(Channel &channel, Client &client);

		/* --- Debug -------------------------------------------------------- */		
		void		_debugListClients();
		void		_debugListChannels();
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
