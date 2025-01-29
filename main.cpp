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
    hints.ai_family = AF_UNSPEC;        // Don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        // Fill in my IP for me

    // Convert port to string
    std::string portStr = std::to_string(myPort);
    if (getaddrinfo(NULL, portStr.c_str(), &hints, &servinfo) != 0)
	{
        std::cerr << "getaddrinfo: failed\n";
        exit(1);
    }

    // Loop through all the results and bind to the first we can
	// Use a copy of servinfo pointer to keep reference to the list's head
    for (tmp = servinfo; tmp != NULL; tmp = p->ai_next)
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




// #include <iostream>
// #include <sys/socket.h>	// socket()
// #include <sys/types.h>	// getaddrinfo()
// #include <netdb.h>		// getaddrinfo()

// #define BACKLOG 5		// how many pending connections queue will hold

// void	runServer(int myPort, std::string password)
// {
// 	struct addrinfo	hints;
// 	struct addrinfo	*servinfo;			// will point to the results

// 	// Load up address structs with getaddrinfo():
// 	memset(&hints, 0, sizeof hints);	// make sure the struct is empty
// 	hints.ai_family = AF_UNSPEC;		// don't care IPv4 or IPv6
// 	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
// 	hints.ai_flags = AI_PASSIVE;		// fill in my IP for me

// 	getaddrinfo(NULL, myPort, &hints, &servinfo);

// 	// Create a socket
// 	int fdSocket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

// 	// Bind it to the port we passed in to getaddrinfo()
// 	bind(fdSocket, res->ai_addr, res->ai_addrlen);

// 	// Start listening
// 	listen(fdSocket, BACKLOG);

// 	// now accept an incoming connection:
// 	struct sockaddr_storage their_addr;
// 	socklen_t addr_size;

// 	addr_size = sizeof their_addr;
// 	int new_fd = accept(fdSocket, (struct sockaddr *)&their_addr, &addr_size);

// }

// int	main(int argc, char **argv)
// {
// 	(void)argc;
// 	(void)argv;
// 	runServer(4242, "pass");

// 	return (0);
// }