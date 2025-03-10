#include "Server.hpp"

void Server::_mode(Client &client, const t_tokens msg)
{
	bool	newstatus = false;
	if (msg.parameters[1][0] == '+')
		newstatus = true;
	char	mode = msg.parameters[1][1];

	if (!_chanExists(msg.parameters[0])) //channel does not exist yet
	{
		this->_sendMessage(client, ERR_NOSUCHCHANNEL(this->_serverName, client.getNickname(), msg.parameters[0]));
		return ;
	}
	
	Channel *channel = _getChannel(msg.parameters[0]);	//copy the channel to an easier parameter "channel"
	//0. CHECK IF client is on that channel
	if (!_onChannel(client, channel->getName()))
	{
		this->_sendMessage(client, ERR_NOTONCHANNEL(this->_serverName, client.getNickname(), channel->getName()));
     	return ;
	}
	//1. CHECK IF client has permission to change modes in the channel
	std::map<std::string, bool>::iterator it = client.getChannels().find(channel->getName());
	if (!it->second) 
	{
    	this->_sendMessage(client, ERR_CHANOPRIVSNEEDED(this->_serverName, client.getNickname(), channel->getName()));
    	return ;
	}
	if (msg.parameters.size() == 2)
	{
		if (msg.parameters[1] == "+i")
			channel->setIonly(true);
		if (msg.parameters[1] == "-i")
			channel->setIonly(false);
		if (mode == 'k')
			channel->setKey("");
		if (mode == 'l')
			channel->setLimit(0);
		if (msg.parameters[1] == "+t")
			channel->setTmode(true);
		if (msg.parameters[1] == "-t")
			channel->setTmode(false);
//		channel.setMode(mode, newstatus);
//		std::cout << "[DEBUG]: channel invite-only mode is now: " << channel.getMode('i') << std::endl;
	}
	else  // is +k, +l OR +/-o
	{
		if (mode == 'k')
			channel->setKey(msg.parameters[2]);
		if (mode == 'l')
		{
			std::stringstream ss(msg.parameters[2]);
			int num;
			ss >> num; //will not store a number out of range, but 
			// Check for successful conversion (fail() and stream state)
			if (ss.fail() || num <= 0 || ss.peek() != EOF) //[1]
			{
   				// Handle invalid input
   				this->_sendMessage(client, ERR_INVALIDMODEPARAM(this->_serverName, \
					client.getNickname(), "l", msg.parameters[2]));
				return ;
			}
			// Check that the limit passed is not lower than current number of clients in the channel
			if (static_cast<size_t>(num) < channel->getClients().size())
			{
				std::stringstream ss;
    			ss << num;

   				this->_sendMessage(client, ERR_CHANNELLIMITTOOLOW(this->_serverName, \
					client.getNickname(), ss.str()));
				return ;
			}
			else
			{
   				channel->setLimit(num); // Only set the limit if it's valid
			}
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
				target->getChannels()[channel->getName()] = newstatus;
			}
			else
			{
				this->_sendMessage(client, ERR_NOSUCHNICK(this->_serverName, client.getNickname(), msg.parameters[2]));
			}
		}
	}
}

Channel	*Server::_getChannel(const std::string &ch_name)
{
	std::map<std::string, Channel>::iterator it;
	for (it = this->_channels.begin(); it != this->_channels.end(); ++it)
	{
		if (it->first == ch_name)
			return(&it->second);
	}
	return (NULL);
}

bool	Server::_onChannel(Client &client, const std::string ch_name)
{
	std::map<std::string, bool>::iterator it;
	for (it = client.getChannels().begin(); it != client.getChannels().end(); ++it)
	{
		if (it->first == ch_name)
			return (true);
	}
	return (false);
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

/*	[1] ss.peek() != EOF, which ensures there are no leftover characters after the number
	(e.g., "123abc" would fail).
*/
