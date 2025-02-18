/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 23:42:08 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/18 18:30:26 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* --- Public Coplien's functions ------------------------------------------- */

// Parameterized constructor
Server::Server(const std::string &port, const std::string &password)
{
	_port = port;
	_password = password;

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

	if ((clientSocket = accept(this->_serverSocket, (struct sockaddr *)&clientAddr, &addr_size)) == -1)
	{
    	closeSockets();
        throw std::runtime_error("[SERVER]: Error: accept() failed");
	}

	std::cout << "[SERVER]: New connection accepted!" << std::endl;

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

	std::string & clientBuffer = currentClient.getBuffer(); // REFERENCE!!

    // Append received data to client's buffer
    clientBuffer.append(buffer, bytesReceived);
	
	// Extract full messages, leaving incomplete ones in clientBuffer
	// We send a REFERENCE!! of clientBuffer, not a copy, to ensure it reflects changes
	// suffered in splitBuffer() (full messages removed, incomplete messages remaining)
	std::vector<std::string> fullMessages = splitBuffer(clientBuffer);
	
	std::cout << "\n[~DEBUG]: Number of full messages stored = " << fullMessages.size() << std::endl;
	// Process each full message
	for (size_t m = 0; m < fullMessages.size(); m++)
	{
		std::cout << "[~DEBUG]: \tfullMessages[" << m << "]: " << fullMessages[m];
		// std::cout << "[DEBUG]: Now it's time TO splitMessage() into command [parameters] [:trailing]\n" << std::endl;
		processMessage(currentClient, fullMessages[m]);
	}	
}

std::vector<std::string> Server::splitBuffer(std::string & buffer)
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

t_tokens	Server::tokenizeMsg(const std::string &message)
{
    std::istringstream	iss;
    std::string 		token;
	t_tokens			tokenizedMsg;

    iss.str(message);

    // Extract command
    if (!(iss >> token))
		return (tokenizedMsg); // Empty message

    tokenizedMsg.command = token;

    // Extract parameters until ":"
    while (iss >> token)
	{
        if (token[0] == ':') // Extract trailing text (here token is storing the colon and the first word)
		{
            std::string trailing;
            std::getline(iss, trailing); // here we extract the rest of trailing line (does not include the colon and the first word)
            tokenizedMsg.trailing = token.substr(1) + trailing; // concatenate first word (colon removed) with resto of trailing
            break;
        }
        tokenizedMsg.parameters.push_back(token); // Add parameter to vector
    }

    return tokenizedMsg;
}

void Server::processMessage(Client &currentClient, std::string message)
{
    t_tokens msgTokens;

    msgTokens = tokenizeMsg(message);

	/* DEBUG PRINTINGS ------------------------------------------------------ */
    std::cout << "[~DEBUG]: \t\tcommand = " << msgTokens.command << std::endl;

    for (size_t j = 0; j < msgTokens.parameters.size(); j++)
    {
        std::cout << "[~DEBUG]: \t\tparameters[" << j << "] = " << msgTokens.parameters[j] << std::endl;
    }

    if (!msgTokens.trailing.empty()) {
        std::cout << "[~DEBUG]: \t\ttrailing = " << msgTokens.trailing << std::endl;
	}
	/* ------------------------------------------------------ DEBUG PRINTINGS */

	if (msgTokens.command == "CAP" && msgTokens.parameters[0] != "END")
	{
		std::string msg = ":localhost CAP * LS :None\r\n";
		send(currentClient.getFd(), msg.c_str(), msg.size(), 0);
	}

	if (msgTokens.command == "PASS")
	{
		if (msgTokens.parameters[0] == this->_password)
			currentClient.setAuthentication(true);
		else
		{
			std::string err_passwdmismatch = "* : Password required\r\n";
			send(currentClient.getFd(), err_passwdmismatch.c_str(), err_passwdmismatch.size(), 0);
		}
	}

	if (msgTokens.command == "NICK")
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

	if (msgTokens.command == "USER")
	{
		if(currentClient.getAuthentication() == true && !(currentClient.getNickname().empty()))
		{
			currentClient.setUsername(msgTokens.parameters[0]);
			currentClient.setRegistration(true);
			std::cout << "\n-> username received, client is registered; your username is " << msgTokens.parameters[0] << std::endl;

			std::string rpl_welcome = "001 " + currentClient.getNickname() + " :Welcome to the Internet Relay Network, " + currentClient.getNickname() + "!\r\n";
			send(currentClient.getFd(), rpl_welcome.c_str(), rpl_welcome.size(), 0);
			std::string rpl_yourhost = "002 " + currentClient.getNickname() + " :Your host is localhost, running version 1.0\r\n";
			send(currentClient.getFd(), rpl_yourhost.c_str(), rpl_yourhost.size(), 0);
			std::string rpl_created = "003 " + currentClient.getNickname() + " :This server was created today\r\n";
			send(currentClient.getFd(), rpl_created.c_str(), rpl_created.size(), 0);
			std::string rpl_myinfo = "004 " + currentClient.getNickname() + " localhost 1.0 -availableusermodes- -availablechannelmodes-\r\n";
			send(currentClient.getFd(), rpl_myinfo.c_str(), rpl_myinfo.size(), 0);	
		}
		else
			std::cout << "client is not authenticated, I will not store the USERNAME" << std::endl;
	}

	// if (currentClient.getAuthentication() == true && currentClient.getRegistration() == true)
	// {
	// 	std::string rpl_welcome = "001 " + currentClient.getNickname() + " :Welcome to the Internet Relay Network, " + currentClient.getNickname() + "!\r\n";
	// 	send(currentClient.getFd(), rpl_welcome.c_str(), rpl_welcome.size(), 0);
	// 	std::string rpl_yourhost = "002 " + currentClient.getNickname() + " :Your host is localhost, running version 1.0\r\n";
	// 	send(currentClient.getFd(), rpl_yourhost.c_str(), rpl_yourhost.size(), 0);
	// 	std::string rpl_created = "003 " + currentClient.getNickname() + " :This server was created today\r\n";
	// 	send(currentClient.getFd(), rpl_created.c_str(), rpl_created.size(), 0);
	// 	std::string rpl_myinfo = "004 " + currentClient.getNickname() + " localhost 1.0 -availableusermodes- -availablechannelmodes-\r\n";
	// 	send(currentClient.getFd(), rpl_myinfo.c_str(), rpl_myinfo.size(), 0);		
	// }
	return;
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
