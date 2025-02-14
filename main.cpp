/* 
	Main TO_DO tasks:
		- Check that argc == 2;
		- Check that argv[0] is a valid port number (must be between 1024 and 65535)
		- Check password requirements, if there's any
		- Check wrong running with ./ircserver localhost 8080 
*/


#include "Server.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (-1);
	}

	Server ircServer(argv[1], argv[2]);
	try
	{
		ircServer.startPoll();
	}
	catch(const std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}