#include "Server.hpp"

void Server::_mode(Client &client, const t_tokens msg)
{
	const t_tokens *ms = &msg;
	//quan n'hi ha un mes del que creiem es pq des de la finestra del canal IRSSI sol ja envia el canal com a parameter[0]
	//std::cout << "[DEBUG] ms->parameters.size() is: " << ms->parameters.size() << std::endl;
	if (_badModeSyntax(ms, client))
		return ;
	if (!_chanExists(msg.parameters[0])) //channel does not exist yet
	{
		if (ms->parameters[0][0] == '#' || ms->parameters[0][0] == '&')
			this->_sendMessage(client, ERR_NOSUCHCHANNEL(this->_serverName, client.getNickname(), msg.parameters[0]));
		return ;
	}

	char	mode = msg.parameters[1][1];
	if (!_chanExists(msg.parameters[0])) //channel does not exist yet
	{
		this->_sendMessage(client, ERR_NOSUCHCHANNEL(this->_serverName, client.getNickname(), msg.parameters[0]));
		return ;
	}
	
	Channel *channel = _getChannel(msg.parameters[0]);	//copy the channel to an easier parameter "channel"
	std::string &server = this->_serverName;
	const std::string &nick = client.getNickname();
	const std::string &ch_name = channel->getName();
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
		{
			if (!validKey(msg.parameters[2]))
			{
				this->_sendMessage(client, ERR_KEYSET(server, nick, ch_name));
				return ; 
			}
			channel->setKey(msg.parameters[2]);
		}
		if (mode == 'l')
		{
			std::stringstream ss(msg.parameters[2]);
			int num;
			ss >> num; //will not store a number out of range, but 
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
		}
		if (mode == 'o')
		{
			bool newstatus = false;
			if (msg.parameters[1][0] == '+')
				newstatus = true;
			//  NEED TO TEST ALL THIS
			std::string targetnick = msg.parameters[2];

//ERRRORRRRRR NO ESTA FUNCIONANT AQUESTA FUNCIOOO
			// we have the nickname of that Client - TEST if this does the sam that the last else in ft
			//if (!_isNickValid(targetnick))
			// {
			// 	std::cout << "[DEBUG]: NICK IS NOT VALID no such nick: " << targetnick << std::endl;

			// 	this->_sendMessage(client, ERR_NOSUCHNICK(server, nick, targetnick));
			// 	return ;
			// }
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
			// CHECK IF the client to be set as +o OR -o is on that channel
			if (target && !_onChannel(*target, ch_name))
				this->_sendMessage(client, ERR_USERNOTINCHANNEL(server, targetnick, ch_name));
			else if (target)
				target->getChannels()[channel->getName()] = newstatus;
			else
			 	this->_sendMessage(client, ERR_NOSUCHNICK(server, nick, targetnick));
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

bool	Server::_badModeSyntax(const t_tokens *ms, Client &client)
{
	bool result = true;
	std::string validModes = "iklto";
	std::string signs = "-+";
	
	//std::cout << "[DEBUG]: ms->parameters.size() is: " << ms->parameters.size() << std::endl;
	//less than 2 params, need 2 at least: channel + mode (WORKING OK)
	if (ms->parameters.size() < 2 || (ms->parameters.size() == 2 &&
		(ms->parameters[1] == "+o" || ms->parameters[1] == "-o" || ms->parameters[1] == "+k" || ms->parameters[1] == "+l")))
		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, ms->parameters[0]));

	//mode has to be 2 chars long, and be sign + validMode (Basic tests OK)
	else if (ms->parameters[1].length() != 2 || signs.find(ms->parameters[1][0]) == std::string::npos
		|| validModes.find(ms->parameters[1][1]) == std::string::npos)
		this->_sendMessage(client, ERR_MODEERROR(this->_serverName, ms->parameters[0], ms->parameters[1]));
	
	//cannot have more than 3 params, and if 3, has to be +o, -o, +l, +k (Basic tests OK)
	else if (ms->parameters.size() > 3 || (ms->parameters.size() == 3 &&
		(ms->parameters[1] != "+o" && ms->parameters[1] != "-o" && ms->parameters[1] != "+k" && ms->parameters[1] != "+l")))
		this->_sendMessage(client, ERR_TOOMANYPARAMS(this->_serverName, ms->parameters[0]));
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
