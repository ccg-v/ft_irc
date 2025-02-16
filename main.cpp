/* 
	Main TO_DO tasks:
		- Check that argc == 2; - DONE
		- Check that argv[0] is a valid port number (must be between 1024 and 65535) - DONE
		- Check password requirements, if there's any - BY IRC PROTOCOL: [5]
*/

#include "Server.hpp"

int	wrongargs(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserc <port> <password>" << std::endl;
		return (1);
	}
	std::string port = argv[1];
	char * endptr;
	long p = std::strtol(port.c_str(), &endptr, 10);
	if (*endptr != '\0')  // Invalid characters found - not a number
	{
        std::cerr << "Error: Port must be a number" << std::endl;
		return (1);
    }
	if (p < 1024 || p > 65535)
	{
        std::cerr << "Error: Port must be 1014 to 65535" << std::endl;
        return (1);
    }
	return (EXIT_SUCCESS);
}

int	main(int argc, char **argv)
{
	if (wrongargs(argc, argv))
		return (EXIT_FAILURE);

	Server ircServer(argv[1], argv[2]);
	try
	{
		ircServer.startPoll();
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
