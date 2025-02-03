#include <iostream>
#include <cstring>      // memset()
#include <cstdlib>      // exit()
#include <sys/socket.h>	// socket(), bind(), listen(), accept()
#include <netdb.h>      // getaddrinfo(), freeaddrinfo()
#include <unistd.h>     // close()
#include <fcntl.h>      // fcntl()
#include <poll.h>       // poll()
#include <vector>		// vector()

#define BACKLOG 5        // How many pending connections queue will hold
#define BUFFER_SIZE 512  // Max buffer size for recv() [1]

void runServer(const std::string myPort, const std::string &password)
{
    struct addrinfo hints;
	struct addrinfo	*servinfo;
	struct addrinfo	*tmp;
    int serverSocket;

    // Load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);    // Make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // Address family unspecified (accept either IPv4 or IPv6)
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        // Fill in my IP for me

    // std::string portStr = std::to_string(myPort);	// Convert port to string
    if (getaddrinfo(NULL, myPort.c_str(), &hints, &servinfo) != 0)	// [2]
	{
        std::cerr << "[SERVER]: getaddrinfo failed" << std::endl;
        exit(-1);
    }

    // Loop through ALL the results and bind to the first we can
	// I use a copy of servinfo pointer to keep reference to the list's head
    for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
	{
        // Create socket
        if ((serverSocket = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol)) == -1)
		{
            std::cerr << "[SERVER]: Socket creation failed" << std::endl;
            continue;
        } else {
			std::cerr << "[SERVER]: Socket created succesfully " << std::endl;
		}

		int opt = 1;

		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			std::cerr << "[SERVER]: setsockopt(SO_REUSEADDR) failed" << std::endl;
			close(serverSocket);
			continue;
		}

        // Bind socket
        if (bind(serverSocket, tmp->ai_addr, tmp->ai_addrlen) == -1)
		{
            close(serverSocket);
            std::cerr << "[SERVER]: Socket binding failed" << std::endl;
            continue;
        } else {
			std::cerr << "[SERVER]: Socket bound succesfully" << std::endl;
		}

        break;  // Socket has been successfully bound
    }

    if (tmp == NULL)
	{
        std::cerr << "[SERVER]: Socket binding failed" << std::endl;
        exit(-1);
    }

    freeaddrinfo(servinfo);  

    // Start listening
    if (listen(serverSocket, BACKLOG) == -1)
	{
        std::cerr << "[SERVER]: Socket failed listening" << std::endl;
        exit(-1);
    }

    std::cout << "[SERVER]: Listening on port " << myPort << "..." << std::endl;

	/*
    // Accept an incoming connection
    struct sockaddr_storage clientAddr;
    socklen_t addr_size = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addr_size);

    if (clientSocket == -1)
	{
        std::cerr << "[SERVER]: Accept failed" << std::endl;
        close(serverSocket);
        exit(-1);
    }

    std::cout << "[SERVER]: New connection accepted!" << std::endl;
	*/

    // Set the listening socket to non-blocking mode
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    // Polling setup
    std::vector<struct pollfd> poll_fds;
    struct pollfd pfd;
	
    pfd.fd = serverSocket;
    pfd.events = POLLIN;
    pfd.revents = 0;
	
    poll_fds.push_back(pfd); // Add the listening socket first

	char buffer[BUFFER_SIZE];

    while (true)
    {
        int poll_count = poll(&poll_fds[0], poll_fds.size(), -1);
        if (poll_count == -1)
        {
            std::cerr << "[SERVER]: Polling error" << std::endl;
            break;
        }
        for (size_t i = 0; i < poll_fds.size(); ++i)
        {
            if (poll_fds[i].revents & POLLIN) // Check for data to read in current socket [3]
            {
                if (poll_fds[i].fd == serverSocket) // Current is the listening socket -> New connection requested
                {
                    struct sockaddr_storage clientAddr;
                    socklen_t addr_size = sizeof(clientAddr);
                    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addr_size);

                    if (clientSocket == -1)
                    {
                        std::cerr << "[SERVER]: Accept failed" << std::endl;
                        continue;
                    }

                    std::cout << "[SERVER]: New connection accepted!" << std::endl;

                    // Set the new socket to non-blocking
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);

					// Create a new struct `pollfd` and push it to `poll_fds` vector of structs
                    struct pollfd new_pfd;
                    new_pfd.fd = clientSocket;
                    new_pfd.events = POLLIN;
                    new_pfd.revents = 0;
                    poll_fds.push_back(new_pfd);
                }
                else // Current is a client socket -> Receive incoming data
                {
                    int bytes_received = recv(poll_fds[i].fd, buffer, BUFFER_SIZE, 0);
                    if (bytes_received <= 0)
                    {
                        std::cerr << "[SERVER]: Connection closed or error" << std::endl;
                        close(poll_fds[i].fd);
                        poll_fds.erase(poll_fds.begin() + i);
                        --i;
                    }
                    else
                    {
                        buffer[bytes_received] = '\0'; // Null-terminate the string
                        std::cout << "[SERVER]: Received: " << buffer << std::endl;

                        // Echo message back to client
                        send(poll_fds[i].fd, buffer, bytes_received, 0);
                    }
                }
            }
        }
    }
	
/*
    // Close sockets
    close(clientSocket);
    close(serverSocket);
*/

    // Close all sockets before exiting
    for (size_t i = 0; i < poll_fds.size(); ++i)
    {
        close(poll_fds[i].fd);
    }
}


/* 
	Main TO_DO tasks:
		- Check that argc == 2;
		- Check that argv[0] is a valid port number (must be between 1024 and 65535)
		- Check password requirements, if there's any
*/
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    runServer("4242", "pass");
    return 0;
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
 *					if (poll_fds[i].revents == POLLIN)
 *			but this would fail if other flags are set (e.g., `POLLIN | POLLHUP` 
 *			would not match).
 */

