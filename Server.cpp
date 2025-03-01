/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 23:42:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/01 23:36:08 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* --- Public Coplien's functions ------------------------------------------- */

// Parameterized constructor
Server::Server(const std::string &port, const std::string &password)
{
	_port = port;
	_password = password;
	_serverName = "42ft_irc";
	_creationTime = getCurrentDate();

	_commandMap["CAP"] = &Server::handleCap;
    _commandMap["PASS"] = &Server::handlePass;
    _commandMap["NICK"] = &Server::handleNick;
    _commandMap["USER"] = &Server::handleUser;
	_commandMap["JOIN"] = &Server::_join;
	_commandMap["PING"] = &Server::_ping;
	_commandMap["PONG"] = &Server::_pong;
	_commandMap["PRIVMSG"] = &Server::_privmsg;

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

    // Loop through ALL the results and bind to the first we can
	// I use a copy of servinfo pointer to keep reference to the list's head

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
}

/* --- Other class methods -------------------------------------------------- */

void	Server::startPoll()
{
    while (true)
    {
        if (poll(&_pollFds[0], _pollFds.size(), -1) == -1)
        {
            throw std::runtime_error("[SERVER]: Error: poll()) failed");
        }
        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            if (_pollFds[i].revents & POLLIN) // Check for data to read in current socket [3]
            {
                if (_pollFds[i].fd == this->_serverSocket) // Current is the listening socket -> New connection requested
                {
					acceptClient();
                }
                else // Current is a client socket -> Receive incoming data
                {
					// receiveRawData(i);
					Client &currentClient = this->_clients[this->_pollFds[i].fd]; // Reference to client in _clients map
					// currentClient.setFd(this->_pollFds[i].fd); // Store the client's fd in our class
    				receiveRawData(currentClient, i);  // Pass the current client reference
                }
            }
        }
    }
	closeSockets();
}

void	Server::acceptClient()
{
	struct sockaddr_storage clientAddr;
	socklen_t 				addr_size;
	int 					clientSocket;

	addr_size = sizeof(clientAddr);

	if ((clientSocket = accept(this->_serverSocket, (struct sockaddr *) &clientAddr, &addr_size)) == -1)
	{
    	closeSockets();
        throw std::runtime_error("[SERVER]: Error: accept() failed");
	}

	// std::cout << "[SERVER]: New connection accepted!" << std::endl;

	// Set the new socket to non-blocking
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		closeSockets();
		throw std::runtime_error("[SERVER]: Error: client's fcntl() failed");
	}

	// Create a new struct `pollfd` and push it to `this->_pollFds` vector of structs
	struct pollfd pfd;
	pfd.fd = clientSocket;
	pfd.events = POLLIN;
	pfd.revents = 0;
	this->_pollFds.push_back(pfd);

	// Instantiate Client object and store it in `_clients` map		
	this->_clients[clientSocket] = Client(clientSocket);

    // Get and store the client's addres, to construct the client's mask later [5]
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

void	Server::receiveRawData(Client &currentClient, size_t &i) // Pass 'i' by reference!!
{
	char	buffer[BUFFER_SIZE];
	size_t	bytesReceived;

	memset(buffer, 0, sizeof(buffer));
	bytesReceived = recv(this->_pollFds[i].fd, buffer, BUFFER_SIZE, 0);

	if (bytesReceived <= 0)
	{
		std::cerr << "[SERVER]: Connection closed or error" << std::endl;
		close(this->_pollFds[i].fd);
		this->_pollFds.erase(this->_pollFds.begin() + i); // erase method expects an iterator
		--i;
		return;
	}

	std::string &clientBuffer = currentClient.getBuffer(); // REFERENCE!!

    // Append received data to client's buffer
    clientBuffer.append(buffer, bytesReceived);
	
	// Extract full messages, leaving incomplete ones in clientBuffer
	// We send a REFERENCE!! of clientBuffer, not a copy, to ensure it reflects changes
	// suffered in splitBuffer() (full messages removed, incomplete messages remaining)
	std::vector<std::string> fullMessages = splitBuffer(clientBuffer);
	
	//std::cout << "\n[~DEBUG]: Number of full messages stored = " << fullMessages.size() << std::endl;
	// Process each full message
	for (size_t m = 0; m < fullMessages.size(); m++)
	{
		//std::cout << "[~DEBUG]: \tfullMessages[" << m << "]: " << fullMessages[m];
		// std::cout << "[DEBUG]: Now it's time TO splitMessage() into command [parameters] [:trailing]\n" << std::endl;
		processMessage(currentClient, fullMessages[m]);
	}	
}

std::vector<std::string> Server::splitBuffer(std::string &buffer)
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

// t_tokens	Server::tokenizeMsg(const std::string &message)
// {
//     std::istringstream	iss;
//     std::string 		token;
// 	t_tokens			tokenizedMsg;// = new t_tokens;

//     iss.str(message);

//     // Extract command
//     if (!(iss >> token))
// 		return (tokenizedMsg); // Empty message

//     tokenizedMsg.command = token;

//     // Extract parameters until ":"
//     while (iss >> token)
// 	{
//         if (token[0] == ':') // Extract trailing text (here token is storing the colon and the first word)
// 		{
//             std::string trailing;
//             std::getline(iss, trailing); // here we extract the rest of trailing line (does not include the colon and the first word)
//             tokenizedMsg.trailing = token.substr(1) + trailing; // concatenate first word (colon removed) with resto of trailing
//             break ;
//         }
// 		if (tokenizedMsg.command == "PRIVMSG" && tokenizedMsg.parameters.size() == 1)
// 		{
//             std::string trailing;
//             std::getline(iss, trailing); // here we extract the rest of trailing line (does not include the colon and the first word)
//             tokenizedMsg.trailing = token + trailing; // concatenate first word (colon removed) with resto of trailing			
// 			break ;
// 		}
//         tokenizedMsg.parameters.push_back(token); // Add parameter to vector
//     }

//     return (tokenizedMsg);
// }

t_tokens	Server::tokenizeMsg(const std::string &message)
{
    std::istringstream	iss;
    std::string 		token;
	t_tokens			tokenizedMsg;// = new t_tokens;

    iss.str(message);

    // Extract command
    if (!(iss >> token))
		return (tokenizedMsg); // Empty message

    tokenizedMsg.command = token;

    // Extract parameters until ":"
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
    return (tokenizedMsg);
}

void Server::processMessage(Client &currentClient, std::string message)
{
    t_tokens msgTokens = tokenizeMsg(message);
	std::map<std::string, void (Server::*)(Client&, t_tokens)>::iterator it = _commandMap.find(msgTokens.command);

	/* DEBUG PRINTINGS ------------------------------------------------------ */
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
	else
	{
		sendMessage(currentClient, ERR_UNKNOWNCOMMAND(this->_serverName, msgTokens.command));
	}
}

void	Server::sendMessage(Client &client, const std::string &message)
{
	send(client.getFd(), message.c_str(), message.size(), 0);
}

void	Server::removeClient(int clientFd)
{ 
	close(clientFd); // Close the client's socket

	this->_clients.erase(clientFd); 	// Remove from _clients map

	// Remove from _pollFds vector
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == clientFd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}

	std::cout << "[SERVER]: Client " << clientFd << " disconnected and removed." << std::endl;
}


void	Server::closeSockets()
{
    for (size_t i = 0; i < this->_pollFds.size(); ++i)
    {
        close(this->_pollFds[i].fd);
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
