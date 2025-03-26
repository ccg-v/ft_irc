#include "Server.hpp"

void Server::_mode(Client &client, const t_tokens msg)
{
	const t_tokens *ms = &msg;
	if (_badModeSyntax(ms, client))
		return ;
	std::string ch_name = toLowerStr(msg.parameters[0]);
	if (!_chanExists(ch_name))
	{
		if (ms->parameters[0][0] == '#' || ms->parameters[0][0] == '&') // protecting MODE sent automatically by IRSSI upon connection
			this->_sendMessage(client, ERR_NOSUCHCHANNEL(this->_serverName, client.getNickname(), ch_name));
		return ;
	}

	char	mode = msg.parameters[1][1];
	Channel *channel = _getChannel(ch_name);
	std::string &server = this->_serverName;
	const std::string &nick = client.getNickname();
	
	//0. CHECK IF client is on that channel
	if (!_onChannel(client, channel->getName()))
	{
		this->_sendMessage(client, ERR_NOTONCHANNEL(server, nick, ch_name));
     	return ;
	}
	//1. CHECK IF client has permission to change modes in the channel
	std::map<std::string, bool>::iterator it = client.getChannels().find(channel->getName());
	if (!it->second) 
	{
    	this->_sendMessage(client, ERR_CHANOPRIVSNEEDED(server, nick, ch_name));
    	return ;
	}
	if (msg.parameters[1] == "+i" || msg.parameters[1] == "-i" || msg.parameters[1] == "-k" || msg.parameters[1] == "-l"
		|| msg.parameters[1] == "+t" || msg.parameters[1] == "-t")
	{
		if (_modeAlreadySet(channel, ms->parameters)) //only to sender bc NO CHANGE has been made
		{
			this->_sendMessage(client, NTC_MODEUNCHANGED(_serverName, nick, ch_name, msg.parameters[1]));
			return ;
		}
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
		//INFORM all channel members about the change
		std::string info = ":" + nick + " MODE " + channel->getName() + " " + msg.parameters[1] + "\r\n";
		std::vector<int> chanClients = channel->getClients();
		for (std::vector<int>::iterator it = chanClients.begin(); it != chanClients.end(); ++it)
		{
			this->_sendMessage_fd(*it, info);
		}
	}
	else  // is +k, +l OR +/-o
	{
		if (mode == 'k')
		{
			if (channel->getKey() == msg.parameters[2]) //only to sender bc NO CHANGE has been made
			{
				this->_sendMessage(client, NTC_KEYUNCHANGED(_serverName, nick, ch_name));
    			return ;
			}
			if (!validKey(msg.parameters[2], 2))
			{
				this->_sendMessage(client, ERR_KEYSET(server, nick, ch_name));
				return ; 
			}
			channel->setKey(msg.parameters[2]);
			//Send message KEY CHANGED TO ALL CHANNEL
			std::string text = ":" + nick + " MODE " + ch_name + " " + msg.parameters[1] + " " + msg.parameters[2] + "\r\n";
			std::vector<int> chanClients = channel->getClients();
			for (std::vector<int>::iterator it = chanClients.begin(); it != chanClients.end(); ++it)
			{
				this->_sendMessage_fd(*it, text);
			}
		}
		if (mode == 'l')
		{
			std::stringstream ss(msg.parameters[2]);
			int num;
			ss >> num; //will not store a number out of range
			if (channel->getLimit() == num) //only to sender bc NO CHANGE has been made
			{
				this->_sendMessage(client, NTC_LIMITUNCHANGED(_serverName, nick, ch_name));
    			return ;
			}
			// Check for successful conversion (fail() and stream state)
			if (ss.fail() || num <= 0 || ss.peek() != EOF) //[1]
			{
   				// Handle invalid input
   				this->_sendMessage(client, ERR_INVALIDMODEPARAM(server, nick, "l", msg.parameters[2]));
				return ;
			}
			// Check that the limit passed is not lower than current number of clients in the channel
			if (static_cast<size_t>(num) < channel->getClients().size())
			{
				std::stringstream ss;
    			ss << num;

   				this->_sendMessage(client, ERR_CHANNELLIMITTOOLOW(server, nick, ss.str()));
				return ;
			}
			channel->setLimit(num); // Only set the limit if it's valid
			//Send message LIMIT SET TO ALL CHANNEL
			std::string message = ":" + nick + " MODE " + ch_name + " " + msg.parameters[1] + msg.parameters[2] + "\r\n";
			std::vector<int> chanClients = channel->getClients();
			for (std::vector<int>::iterator it = chanClients.begin(); it != chanClients.end(); ++it)
			{
				this->_sendMessage_fd(*it, message);
			}
		}
		if (mode == 'o')
		{
			bool newstatus = false;
			if (msg.parameters[1][0] == '+')
				newstatus = true;
			std::string targetnick = msg.parameters[2];
			Client 		*target = NULL;

			// target is the target Client to change to isOp or isNotOp status (true)
			for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
			{
				if (it->second.getNickname() == targetnick)
				{
					target = &(it->second);
					break ;
				}
			}
			if (target && !_onChannel(*target, ch_name))
				this->_sendMessage(client, ERR_USERNOTINCHANNEL(server, targetnick, ch_name));
			//send NOTICE ONLY to sender if the target is already Op
			else if (target && newstatus == true && target->isOperator(ch_name))
			{
				this->_sendMessage(client, NTC_ALREADYOP(_serverName, nick, ch_name, targetnick));
				return ;
			}
			//send NOTICE ONLY to sender if the target is already NotOp
			else if (target && newstatus == false && !target->isOperator(ch_name))
			{
				this->_sendMessage(client, NTC_NOTOP(_serverName, nick, ch_name, targetnick));
				return ;
			}
			else if (target)
			{
				target->getChannels()[channel->getName()] = newstatus;
				//Send message ABOUT THE CHANGE TO ALL CHANNEL
				std::string message = ":" + nick + " MODE " + ch_name + " " + msg.parameters[1] + " " + msg.parameters[2] + "\r\n";
				std::vector<int> chanClients = channel->getClients();
				for (std::vector<int>::iterator it = chanClients.begin(); it != chanClients.end(); ++it)
				{
					this->_sendMessage_fd(*it, message);
				}
			}
			else
			 	this->_sendMessage(client, ERR_NOSUCHNICK(server, nick, targetnick));
		}
	}
}

bool	Server::_modeAlreadySet(Channel *channel, const std::vector<std::string> &params)
{
	std::string mode = params[1];
	if ((mode == "+i" && channel->getIonly()) || (mode == "-i" && !channel->getIonly()))
		return (true);
	else if ((mode == "+t" && channel->getTmode()) || (mode == "-t" && !channel->getTmode()))
		return (true);
	else if ((mode == "-k" && channel->getKey() == "") || (mode == "-l" && !channel->getLimit()))
		return (true);
	return (false);
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

bool	Server::_badModeSyntax(const t_tokens *ms, Client &client)
{
	bool result = true;
	std::string validModes = "iklto";
	std::string signs = "-+";
	
	//mode has to be 2+ chars long, and be sign + validMode (Basic tests OK), if >2, 3rd and further are dismissed by most servers
	if (ms->parameters.size() > 1 && (ms->parameters[1].length() != 2 || signs.find(ms->parameters[1][0]) == std::string::npos
		|| validModes.find(ms->parameters[1][1]) == std::string::npos))
		this->_sendMessage(client, ERR_MODEERROR(this->_serverName, ms->parameters[0], ms->parameters[1]));
	//less than 2 params, need 2 at least: mode + channel
	else if (ms->parameters.size() < 2 || (ms->parameters.size() < 3 &&
		(ms->parameters[1] == "+o" || ms->parameters[1] == "-o" || ms->parameters[1] == "+k" || ms->parameters[1] == "+l")))
		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, ms->parameters[0]));
	else
		result = false;
	return (result);
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
