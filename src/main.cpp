#include "Server.hpp"

int	wrongargs(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserc <port> <password>" << std::endl;
		return (EXIT_FAILURE);
	}
	std::string port = argv[1];
	char * endptr;
	long p = std::strtol(port.c_str(), &endptr, 10);
	if (*endptr != '\0')  // Invalid characters found - not a number
	{
        std::cerr << "Error: Port must be a number" << std::endl;
		return (EXIT_FAILURE);
    }
	if (p < 1024 || p > 65535)
	{
        std::cerr << "Error: Port must be 1024 to 65535" << std::endl;
        return (EXIT_FAILURE);
    }
	// Validate password
	if (!validKey(argv[2], 8))
	{
		std::cerr << "Password must be 8-32 chars long and contain only alphanumeric chars or $%&'()*+-.@" << std::endl;
		return (EXIT_FAILURE);
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
