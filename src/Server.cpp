/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 23:42:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/26 20:13:40 by erosas-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	signalCaught = false;

// Handle server's termination with SIGINT(ctrl + C) or SIGQUIT (ctrl + \)
void signalHandler(int signal) {
	(void) signal; 
	signalCaught = true;

	std::ostringstream msg;
    msg << "\n[SERVER]: " << strsignal(signal) << " received, closing server...";
    std::cerr << msg.str() << std::endl;
}

/* --- Public Coplien's functions ------------------------------------------- */

// Parameterized constructor
Server::Server(const std::string &port, const std::string &password)
{
	_port = port;
	_password = password;
	_serverName = "42ft_irc";
	_creationTime = getCurrentDate();

	_commandMap["CAP"] = &Server::_cap;
    _commandMap["PASS"] = &Server::_pass;
    _commandMap["NICK"] = &Server::_nick;
    _commandMap["USER"] = &Server::_user;
	_commandMap["JOIN"] = &Server::_join;
    _commandMap["MODE"] = &Server::_mode;
	_commandMap["PING"] = &Server::_ping;
	_commandMap["PONG"] = &Server::_pong;
	_commandMap["PRIVMSG"] = &Server::_privmsg;
	_commandMap["KICK"] = &Server::_kick;
	_commandMap["INVITE"] = &Server::_invite;
	_commandMap["TOPIC"] = &Server::_topic;
	_commandMap["PART"] = &Server::_part;
	_commandMap["QUIT"] = &Server::_quit;
	_commandMap["DEBUG"] = &Server::_debug;
	_commandMap["NOTICE"] = &Server::_privmsg;

    struct addrinfo hints;
	struct addrinfo	*servinfo;
	struct addrinfo	*tmp;

    // Load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);    // Make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // Address family unspecified (accept either IPv4 or IPv6)
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        // Fill in my IP for me

    if (getaddrinfo(NULL, _port.c_str(), &hints, &servinfo) != 0)	// [2]
	{
        throw std::runtime_error("[SERVER]: Error: getaddrinfo() failed");
    }

	std::string lastError;

    for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
	{
        // Create socket
        if ((this->_serverSocket = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol)) == -1)
		{
            lastError = "socket() failed";
            continue;
        } 

		int opt = 1;
		if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			lastError = "setsockopt() failed";
			close(this->_serverSocket);
			continue;
		}

        // Bind socket
        if (bind(this->_serverSocket, tmp->ai_addr, tmp->ai_addrlen) == -1)
		{
            lastError = "bind() failed";
            close(this->_serverSocket);
            continue;
        }

        break;  // Socket has been successfully bound
    }

    if (tmp == NULL)
        throw std::runtime_error("[SERVER]: All socket binding attempts failed: " + lastError);

    freeaddrinfo(servinfo);  

    // Start listening
    if (listen(this->_serverSocket, BACKLOG) == -1)
	{
		close(this->_serverSocket);
        throw std::runtime_error("[SERVER]: Error: listen() failed");
    }

    std::cout << "[SERVER]: Listening on port " << _port << "..." << std::endl;

    // Set the listening socket to non-blocking mode
	if (fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		close(this->_serverSocket);
		throw std::runtime_error("[SERVER]: Error: server's fcntl() failed");
	}
	
    // Polling setup
    struct pollfd pfd;
	
    pfd.fd = this->_serverSocket;
    pfd.events = POLLIN;
    pfd.revents = 0;
	
    _pollFds.push_back(pfd); // Add the listening socket first
}

// Default destructor
Server::~Server()
{
	this->_removeAllClients();
	
    std::cerr << "[SERVER]: Destructor called: all clients removed, server closed. Bye!" << std::endl;
}

/* --- Other class methods -------------------------------------------------- */

void	Server::startPoll()
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGQUIT, signalHandler);

    while (!signalCaught)
    {
        if (poll(&_pollFds[0], _pollFds.size(), -1) == -1)
        {
			if (signalCaught)
			{
				_removeAllClients();
			}
			else
				throw std::runtime_error("[SERVER]: Error: poll()) failed");
        }
		
        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            if (_pollFds[i].revents & POLLIN) // Check for data to read in current socket [3]
            {
                if (_pollFds[i].fd == this->_serverSocket) // Current is the listening socket -> New connection requested
                {
					if (this->_clients.size() < BACKLOG)
					{
						_acceptClient();
					}
					else
					{
						int	refusedClient = accept(this->_serverSocket, NULL, NULL);
						std::string message = INF_SERVERISFULL(this->_serverName);
						send(refusedClient, message.c_str(), message.size(), 0);
						close(refusedClient);
					}
                }
                else // Current is a client socket -> Receive incoming data
                {
					Client &currentClient = this->_clients[this->_pollFds[i].fd]; // Reference to client in _clients map
    				_receiveRawData(currentClient, i);  // Pass the current client reference
                }
            }
        }
		_checkGhostClients();
    }
	_closeSockets();
}

void	Server::_acceptClient()
{
	struct sockaddr_storage clientAddr;
	socklen_t 				addr_size;
	int 					clientSocket;

	addr_size = sizeof(clientAddr);

	if ((clientSocket = accept(this->_serverSocket, (struct sockaddr *) &clientAddr, &addr_size)) == -1)
	{
    	_closeSockets();
        throw std::runtime_error("[SERVER]: Error: accept() failed");
	}

	// Set the new socket to non-blocking
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		_closeSockets();
		throw std::runtime_error("[SERVER]: Error: client's fcntl() failed");
	}

	// Create a new struct `pollfd` and push it to `this->_pollFds` vector of structs
	struct pollfd pfd;
	pfd.fd = clientSocket;
	pfd.events = POLLIN; // poll() watches for incoming data and hang ups
	pfd.revents = 0;
	this->_pollFds.push_back(pfd);

	// Instantiate Client object and store it in `_clients` map		
	this->_clients[clientSocket] = Client(clientSocket);

    // Get and store the client's address, to construct the client's mask later [5]
    char ipStr[INET6_ADDRSTRLEN];

    if (clientAddr.ss_family == AF_INET)
    {
        struct sockaddr_in *addr = (struct sockaddr_in*)&clientAddr;
		strcpy(ipStr, inet_ntoa(addr->sin_addr)); // Copy the IPv4 string
        std::cout << "[SERVER]: New client connected from IPv4: " << ipStr << std::endl;
    }
    else
    {
		// struct sockaddr_in6 *addr = (struct sockaddr_in6*)&clientAddr;
		strcpy(ipStr, "IPv6_client");
        std::cout << "[SERVER]: New client connected from IPv6" << std::endl;
    }

	// Store IP in Client object
	this->_clients[clientSocket].setClientIp(ipStr);
}

void	Server::_receiveRawData(Client &currentClient, size_t &i) // Pass 'i' by reference!!
{
	char	buffer[BUFFER_SIZE];
	size_t	bytesReceived;

	memset(buffer, 0, sizeof(buffer));
	bytesReceived = recv(this->_pollFds[i].fd, buffer, BUFFER_SIZE, 0);

	if (bytesReceived <= 0)
	{
		int	clientFd = this->_pollFds[i].fd;

		_removeClient(clientFd);
		--i;
		return;
	}

	std::string &clientBuffer = currentClient.getBuffer(); // REFERENCE!!

	if (clientBuffer.size() + bytesReceived > 512) 
	{
		_sendMessage(currentClient, ERR_INPUTTOOLONG(this->_serverName, currentClient.getNickname()));
		clientBuffer.clear(); // Clear the buffer to prevent partial overflowed messages
		return;
	}

    // Append received data to client's buffer
    clientBuffer.append(buffer, bytesReceived);

	// Extract full messages, leaving incomplete ones in clientBuffer
	// We send a REFERENCE!! of clientBuffer, not a copy, to ensure it reflects changes
	// suffered in _splitBuffer() (full messages removed, incomplete messages remaining)
	std::vector<std::string> fullMessages = _splitBuffer(clientBuffer);
	
	// Process each full message
	for (size_t m = 0; m < fullMessages.size(); m++)
	{
		_processMessage(currentClient, fullMessages[m]);
	}	
}

std::vector<std::string> Server::_splitBuffer(std::string &buffer)
{
    std::vector<std::string> fullMessages;
    size_t pos = 0;

    while ((pos = buffer.find("\r\n")) != std::string::npos)
	{
        fullMessages.push_back(buffer.substr(0, pos + 2));  // Extract full message
        buffer.erase(0, pos + 2);  // Remove processed part from buffer
    }
    return fullMessages;  // Remaining (incomplete) data stays in buffer
}

t_tokens	Server::_tokenizeMsg(const std::string &message)
{
    std::istringstream	iss;
    std::string 		token;
	t_tokens			tokenizedMsg;// = new t_tokens;

    iss.str(message);

    // Extract command
    if (!(iss >> token))
		return (tokenizedMsg); // Empty message

    tokenizedMsg.command = token;

    // Extract parameters until ":", or after first parameter if command is PRIVMSG
    while (iss >> token)
	{
        if (token[0] == ':' || (tokenizedMsg.command == "PRIVMSG" && tokenizedMsg.parameters.size() == 1)) // Extract trailing text (here token is storing the colon and the first word)
		{
            std::string trailing;
            std::getline(iss, trailing); // here we extract the rest of trailing line (does not include the colon and the first word)
			if (token[0] == ':')
            	tokenizedMsg.trailing = token.substr(1) + trailing; // concatenate first word (colon removed) with rest of trailing
			else
				tokenizedMsg.trailing = token + trailing; // the same but without substracting first char because there is no colon
            break ;
        }
		tokenizedMsg.parameters.push_back(token); // Add parameter to vector
	}

	if (!tokenizedMsg.trailing.empty() && tokenizedMsg.trailing[tokenizedMsg.trailing.size() - 1] == '\r')
		tokenizedMsg.trailing.erase(tokenizedMsg.trailing.size() - 1);

    return (tokenizedMsg);
}

void Server::_processMessage(Client &currentClient, std::string message)
{
    t_tokens msgTokens = _tokenizeMsg(message);
	std::map<std::string, void (Server::*)(Client&, t_tokens)>::iterator it = _commandMap.find(msgTokens.command);

	/* DEBUG PRINTINGS ------------------------------------------------------ */
	std::cout << std::endl;	

	std::cout << "[~DEBUG]: " << message;
    std::cout << "[~DEBUG]: \tcommand = " << msgTokens.command << std::endl;

    for (size_t j = 0; j < msgTokens.parameters.size(); j++)
    {
        std::cout << "[~DEBUG]: \tparameters[" << j << "] = " << msgTokens.parameters[j] << std::endl;
    }

    if (!msgTokens.trailing.empty())
	{
        std::cout << "[~DEBUG]: \ttrailing = " << msgTokens.trailing << std::endl;
	}
	std::cout << std::endl;
	/* ------------------------------------------------------ DEBUG PRINTINGS */

	if (it != _commandMap.end())
	{
		(this->*(it->second))(currentClient, msgTokens);  // Call the correct handler function
	}
	else if (!msgTokens.command.empty())
	{
		_sendMessage(currentClient, ERR_UNKNOWNCOMMAND(this->_serverName, msgTokens.command));
	}
}

void	Server::_sendMessage(Client &client, const std::string &message)
{
	send(client.getFd(), message.c_str(), message.size(), 0);
}

void	Server::_sendMessage_fd(int fd, const std::string &message)
{
	send(fd, message.c_str(), message.size(), 0);
}

void Server::_broadcastToChannel(Channel &channel, const std::vector<std::string> &messages)
{
    for (size_t i = 0; i < channel.getClients().size(); i++)
    {
        Client *member = _findClientByFd(channel.getClients()[i]);
        
        for (size_t j = 0; j < messages.size(); j++)
        {
            _sendMessage(*member, messages[j]);
        }
    }
}

void Server::_checkGhostClients()
{ 
    time_t now = time(NULL);
    std::map<int, Client>::iterator it = this->_clients.begin();

    while (it != this->_clients.end())
    {
        if ((!it->second.getAuthentication() || !it->second.getRegistration()) && (now - it->second.getStartTime()) > TIMEOUT)
        {
            int fd = it->second.getFd();
            std::cout << "[~DEBUG]: Disconnecting ghost client on fd " << fd
                      << " (registration timeout)" << std::endl;
            close(fd);

            // Remove fd from poll list
            for (std::vector<struct pollfd>::iterator pollIt = _pollFds.begin(); pollIt != _pollFds.end(); ++pollIt)
            {
                if (pollIt->fd == fd)
                {
                    _pollFds.erase(pollIt);
                    break;
                }
            }

            // Safely erase the client from _clients
            std::map<int, Client>::iterator next = it;
            ++next;
            this->_clients.erase(it);
            it = next;
        }
        else
        {
            ++it;
        }
    }
}


void	Server::_removeClient(int clientFd)
{

	std::cerr << "[SERVER]: Client " << clientFd << " (" << _clients[clientFd].getHostMask() \
			  << ") disconnected and removed." << std::endl;
	if (signalCaught)
	{
		_sendMessage(this->_clients[clientFd], NTC_SERVERSHUTDOWN(this->_serverName, this->_clients[clientFd].getNickname()));
	}
	else
	{
		_sendMessage(this->_clients[clientFd], INF_CLIENTQUIT(this->_serverName, this->_clients[clientFd].getNickname()));
	}
			  
	// 1. Close the client's socket
	close(clientFd); 

	// 2. Remove from _clients map
	this->_clients.erase(clientFd);

	// 3. Remove from _pollFds vector
	std::vector<struct pollfd>::iterator it;

	for (it = this->_pollFds.begin(); it != this->_pollFds.end(); it++)
	{
		if (it->fd == clientFd)
		{
			this->_pollFds.erase(it);
			break;
		}
	}
}

void Server::_removeAllClients()
{
	if (this->_clients.empty())
		return ;

	std::cerr << "[SERVER]: Removing all clients..." << std::endl;
	
    while (!this->_clients.empty()) 
    {
        this->_removeClient(this->_clients.begin()->first);
    }
}

void	Server::_closeSockets()
{
    while (!this->_clients.empty()) 
    {
        this->_removeClient(this->_clients.begin()->first);
    }
}

/*	[1] Protocols RFC 1459 and RFC 2812, section 2.3:
 *
 *		"IRC messages are always lines of characters terminated with a CR-LF
 *		(Carriage Return - Line Feed) pair, and these messages shall not exceed
 *		512 characters in length, counting all characters including the trailing
 *		CR-LF. Thus, there are 510 characters maximum allowed for the command
 *		and its parameters.""
 */

/* 
 *	[2]	The first parameter of getaddrinfo() is the hostname or IP address that
 *		we want to resolve. The NULL value means that we want to bind to any 
 *		available network interface on this machine, not to an specific address.
 *		This is typical when setting up a server, since it should accept 
 *		connections from multiple interfaces.
 *		Therefore, if we want to bind to an specific IP address, we must replace
 *		the NULL with an IP address as a string. If we want only local connections
 *		(from the same machine) to reach the server (loopback only), we must 
 *		replace the NULL with the localhost address ("127.0.0.1").
 */

/*
 *	[3] - `revents` is a bitmask storing multiple event flags (e.g., POLLIN,  
 *			POLLOUT,POLLHUP can be set at the same time).
 *		- The expression `revents & POLLIN` is a bitwise AND operation that 
 *			checks if the POLLIN bit is set inside `revents`.
 *		- There is not a direc, safe, non-bitwise way to check if `revents` is 
 *			POLLIN. We could check it with:
 *					if (this->_pollFds[i].revents == POLLIN)
 *			but this would fail if other flags are set (e.g., `POLLIN | POLLHUP` 
 *			would not match).
 */

/*
 *	[4] This is a simple echo server behavior, where whatever the client sends is 
 *		returned exactly as it was received. The number of bytes sent is the same
 *		as the number of bytes received.
 */

 /*
  * [5] 	std::string(buffer, bytesReceived);
  *
  *		'buffer' is a char array (C style string). This line constructs a new 
  *		std::string by copying exactly 'bytesReceived' characters from buffer, even
  *		if it contains nullcharacters ('\0').
  *		Unlike std::string(buffer), which stops at the first '\0', this constructor
  *		ensures that all bytesRead characters are included in the string.
  */

 /*
  *	[6]	The subject allows communication with either IPv4 or IPv6. However, this 
  *		becomes a tricky situation because we need to use the host to build the
  *		client's mask:
  *							<nick>!<user>@host
  *		The address is stored in binary format in a struct of type `sockaddr_in`
  *		(IPv4) or `sockaddr_in6` (IPv6). Getting it in a readable form is easy:
  *		- use `inet_ntoa()` for IPv4;
  *		- use `inet_ntop()` for IPv6;
  *		The problem is that `inet_ntop()` is not mentioned in the subject as one
  *		of the allowed functions.
  *		A possibility is to manually format the IPv6 address from binary to string
  *		using `sprintf()`
  */
