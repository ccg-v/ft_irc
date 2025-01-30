#include <iostream>
#include <cstring>       // memset()
#include <cstdlib>       // exit()
#include <sys/socket.h>  // socket(), bind(), listen(), accept()
#include <netdb.h>       // getaddrinfo(), freeaddrinfo()
#include <unistd.h>      // close()

#define BACKLOG 5        // How many pending connections queue will hold

void runServer(int myPort, const std::string &password)
{
    struct addrinfo hints;
	struct addrinfo	*servinfo;
	struct addrinfo	*tmp;
    int fdSocket;

    // Load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);    // Make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // Address family unspecified, could be IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        // Fill in my IP for me

    // Convert port to string
    std::string portStr = std::to_string(myPort);
    if (getaddrinfo(NULL, portStr.c_str(), &hints, &servinfo) != 0)	// (1)
	{
        std::cerr << "getaddrinfo: failed\n";
        exit(1);
    }

    // Loop through all the results and bind to the first we can
	// I use a copy of servinfo pointer to keep reference to the list's head
    for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
	{
        // Create socket
        if ((fdSocket = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol)) == -1)
		{
            std::cerr << "socket: failed\n";
            continue;
        }

        // Bind socket
        if (bind(fdSocket, tmp->ai_addr, tmp->ai_addrlen) == -1)
		{
            close(fdSocket);
            std::cerr << "bind: failed\n";
            continue;
        }

        break;  // Successfully bound
    }

    if (tmp == NULL)
	{
        std::cerr << "Failed to bind socket\n";
        exit(2);
    }

    freeaddrinfo(servinfo);  

    // Start listening
    if (listen(fdSocket, BACKLOG) == -1)
	{
        std::cerr << "listen: failed\n";
        exit(3);
    }

    std::cout << "Server is listening on port " << myPort << "\n";

    // Accept an incoming connection
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof their_addr;
    int new_fd = accept(fdSocket, (struct sockaddr *)&their_addr, &addr_size);

    if (new_fd == -1)
	{
        std::cerr << "accept: failed\n";
        close(fdSocket);
        exit(4);
    }

    std::cout << "New connection accepted!\n";

    // Close sockets
    close(new_fd);
    close(fdSocket);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    runServer(4242, "pass");
    return 0;
}

/* 
 *	(1)	The first parameter of getaddrinfo() is the hostname or IP address that
 *		we want to resolve. The NULL value means that we want to bind to any 
 *		available network interface on this machine, not to an specific address.
 *		This is typical when setting up a server, since it should accept 
 *		connections from multiple interfaces.
 *		Therefore, if we want to bind to an specific IP address, we must replace
 *		the NULL with an IP address as a string. If we want only local connections
 *		(from the same machine) to reach the server (loopback only), we must 
 *		replace the NULL with the localhost address ("127.0.0.1").
 */


