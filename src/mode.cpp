#include "Server.hpp"

// IN CHANNEL: std::map<char, bool>	_modes;
// IN SERVER: std::map<std::string, Channel>	_channels; // key in the map is a channel's name

void Server::_mode(Client &client, const t_tokens msg)
{
	// #ifndef DEBUG
	// 	std::cout << "[~DEBUG]: ENTERING MODE FT - command is: " << msg.command << std::endl;
	// 	std::cout << "parameter 0 is: " << msg.parameters[0] << std::endl;
	// 	std::cout << "parameter 1 is: " << msg.parameters[1] << std::endl;
	// #endif

	bool	newstatus = false;
	if (msg.parameters[1][0] == '+')
		newstatus = true;
	char	mode = msg.parameters[1][1];

	// Check if channel DOES NOT exists
	std::map<std::string, Channel>::iterator it = this->_channels.find(msg.parameters[0]);
	if (it == this->_channels.end())  // Channel does not exist
	{
	//	std::cout << "[~DEBUG]: channel doesn't exist" << std::endl;
    	this->_sendMessage(client, ERR_NOSUCHCHANNEL(this->_serverName, client.getNickname(), msg.parameters[0]));
    	return ;
	}

	Channel &channel = it->second; //MUST BE A REFERENCE to make sure any change to the channel is accessible from "channel"
	//0. CHECK IF client is on that channel
	//DEBUGGING client.getChannels(0 function)
	std::cout << "[~DEBUG]: channels the client is connected to: " << std::endl;
	for (std::map<std::string, bool>::iterator it = client.getChannels().begin(); it != client.getChannels().end(); ++it)
	{
    	std::cout << "Client channel: " << it->first << " | Is op: " << it->second << std::endl;
	}
	//std::map<std::string, bool>::iterator it = client.getChannels().find(channel.getName());
	std::cout << "[~DEBUG]: typeid of getChannels: " << typeid(client.getChannels()).name() << std::endl;
	// if (it == client.getChannels().end()) 
	// {
	// //	std::cout << "[~DEBUG]: channel doesn't exist" << std::endl;
    // 	this->_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), channel.getName()));
    // 	return ;
	// }
	
	// //1. CHECK IF client has permission to change modes in the channel
	// std::map<std::string, bool>::iterator it = client.getChannels().find(channel.getName());
	// if (it == client.getChannels().end()) 
	// {
	// //	std::cout << "[~DEBUG]: channel doesn't exist" << std::endl;
    // 	this->_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), channel.getName()));
    // 	return ;
	// }
	if (msg.parameters.size() == 2)
	{
		channel.setMode(mode, newstatus);
//		std::cout << "[DEBUG]: channel invite-only mode is now: " << channel.getMode('i') << std::endl;
	}
	else  // is +k, +l OR +/-o
	{
		if (mode == 'k')
			channel.setKey(msg.parameters[2]);
		if (mode == 'l')
		{
			std::stringstream ss(msg.parameters[2]);
			int num;
			ss >> num;
			channel.setLimit(num);
		}
		if (mode == 'o')
		{
			//  NEED TO TEST ALL THIS

			std::string targetnick = msg.parameters[2];
			Client 		*target = NULL;
			//target is the target Client to change to isOp or isNotOp status (true)
			// we have the nickname of that Client  //APROFITAR FUNCIO EXISTENT CARLES a nick.cpp (Server::_nickExists)
			for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
			{
				if (it->second.getNickname() == targetnick)
				{
					target = &(it->second);
					break ;
				}
			}
			if (target)
			{
				target->getChannels()[channel.getName()] = newstatus;
			}
			else
			{
				this->_sendMessage(client, ERR_NOSUCHNICK(this->_serverName, client.getNickname(), msg.parameters[2]));
			}
		}
	}
}


/* For channel operators, the MODE command is used as follows:

	/mode <#channel> <mode> [parameter]

	Channel Modes:
		+i / -i → Enable/disable invite-only mode.
			Example: /mode #chat +i
		+t / -t → Restrict topic changes to operators only.
			Example: /mode #chat +t
		+k <key> / -k → Set/remove channel password.
			Example: /mode #chat +k secretpass
			Example: /mode #chat -k
		+o <nick> / -o <nick> → Give/remove operator privileges.
			Example: /mode #chat +o Alice
			Example: /mode #chat -o Bob
		+l <number> / -l → Set/remove user limit.
			Example: /mode #chat +l 10
			Example: /mode #chat -l
	
	Only channel operators can use these commands.

The MODE command in IRC only affects one channel per command. If a client wants to change modes
for multiple channels, they must send separate MODE commands for each channel.
	For example, these must be sent separately:
		/mode #chat1 +i
		/mode #chat2 +i
	You cannot do something like:
		/mode #chat1 #chat2 +i  (❌ Invalid)
Each MODE command only applies to a single channel at a time.
*/
