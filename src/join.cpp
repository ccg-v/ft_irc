#include "Server.hpp"

void Server::_join(Client &client, const t_tokens msg) //[1]
{
	std::vector<std::string>	chs2join; // in case there are several channels to join
    std::vector<std::string>	keys;	// if there are several channels with several keys
	bool somekey = msg.parameters.size() > 1 ? true : false; //true if >1 parameters; means client sent some key to some channel

	if (msg.parameters.size() < 1)
	{
		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, msg.command));
		return ;
	}
	chs2join = splitByComma(msg.parameters[0]);
	if (somekey)
	    keys = splitByComma(msg.parameters[1]);
	size_t n = chs2join.size();
	for (size_t i = 0; i < n; i++)
	{
		std::string ch_name = toLowerStr(chs2join[i]);
		std::string	key = "";
		if (somekey && i < keys.size() && keys[i] != "x")
			key = keys[i];
		if (key != "" && !validKey(key, 2))
		{
			this->_sendMessage(client, ERR_KEYSET(_serverName, client.getNickname(), ch_name));
			continue ;
		}
		if (client.getChannels().size() >= MAXCHAN)
		{
			this->_sendMessage(client, ERR_TOOMANYCHANNELS(this->_serverName, client.getNickname(), ch_name));
    	   	return;
    	}
		if (!this->_chanExists(ch_name)) //CHANNEL DOES NOT EXIST - NEED TO CREATE IT
		{
			if (!this->_validChannelName(ch_name))
			{
				this->_sendMessage(client, ERR_BADCHANMASK(this->_serverName, ch_name));
				return;
			}
			Channel newChannel(ch_name, key); //instantiate the channel
			newChannel.addClient(client.getFd()); //add client to the Channel clients vector
			this->_channels[ch_name] = newChannel; //Store the MODIFIED channel to the Server channels map
			//JOIN message to the creator (only current member)
			std::string ms = ":" + client.getNickname() + " JOIN " + ch_name + "\r\n";
			this->_sendMessage(client, ms);
		
			client.addChannel(ch_name, true); //add the channel to the client's channels map
			//MODE message to the creator
			std::string msg = ":" + this->_serverName + " MODE " + ch_name + " +o " + client.getNickname() + "\r\n";
			this->_sendMessage(client, msg);
		}
		else  //channel exists
		{
			Channel &channel = this->_channels[ch_name];
			if (this->_onChannel(client, ch_name)) //NOT SHOWING ON IRSSI (which manages that itself)
				this->_sendMessage(client, ERR_USERONCHANNEL(this->_serverName, client.getNickname(), ch_name));
		    // Check that channel is NOT inviteOnly OR user has an invitation to it in case it is iOnly
			else if (channel.getIonly() && !_isInvited(channel, client.getFd()))
                this->_sendMessage(client, ERR_INVITEONLYCHAN(this->_serverName, client.getNickname(), ch_name));
			else if (channel.getKey() != "" && key != channel.getKey())
				this->_sendMessage(client, ERR_BADCHANNELKEY(this->_serverName, client.getNickname(), ch_name));
			else if (channel.getLimit() && channel.getClients().size() == static_cast<size_t>(channel.getLimit()))
				this->_sendMessage(client, ERR_CHANNELISFULL(this->_serverName, client.getNickname(), ch_name));
			//JOIN THE CLIENT TO THE CHANNEL with 'false' as operator, except when channel is empty:
			// 1. Add the channel to the list of channels for that client, 2. Add the client to the list of clients for the channel 
			else
			{
				if (channel.getClients().size() == 0)
					client.addChannel(ch_name, true); //if the channel is empty, the fist client to join it becomes its operator
				else
					client.addChannel(ch_name, false); //add the channel to the client's channels map
				channel.addClient(client.getFd()); //add client to the Channel clients vector
				// invite disappears when client joins
				if (_isInvited(channel, client.getFd()))
				{
					std::vector<int> &invited = channel.getInvited();
					invited.erase(std::remove(invited.begin(), invited.end(), client.getFd()), invited.end());
				}
				//JOIN message to ALL CLIENTS in the channel
				_joinMsgToAll(client, this->_channels[ch_name]);
				//TOPIC MESSAGE only to the joiner
				if (this->_channels[ch_name].getTopic() != "")
					this->_sendMessage(client, RPL_TOPIC(this->_serverName, client.getNickname(), ch_name, this->_channels[ch_name].getTopic()));
				else
					this->_sendMessage(client, RPL_NOTOPIC(this->_serverName, client.getNickname(), ch_name));
			}
		}
	}
}

void	Server::_joinMsgToAll(Client &joiner, Channel &channel)
{
	std::string ms = ":" + joiner.getHostMask() + " JOIN " + channel.getName() + "\r\n";
	std::vector<int> chanClients = channel.getClients();
	for (std::vector<int>::iterator it = chanClients.begin(); it != chanClients.end(); ++it)
	{
		this->_sendMessage_fd(*it, ms);
	}
}

bool Server::_isInvited(Channel &channel, int client_fd)
{
	std::vector<int> fds = channel.getInvited();
	for (std::vector<int>::iterator it = fds.begin(); it != fds.end(); ++it)
	{
    	if (*it == client_fd)
        	return (true);
	}
	return (false);
}

bool	Server::_chanExists(const std::string &name)
{  
    std::map<std::string, Channel>::iterator it;
    for (it = this->_channels.begin(); it != this->_channels.end(); ++it)
    {
        if (toLowerStr(it->first) == toLowerStr(name))
            return (true);
    }
    return (false);
}

bool Server::_validChannelName(std::string &name)
{
    // Check if the name is empty or if it doesn't start with either '#' or '&'
    if (name.empty() || (name[0] != '#' && name[0] != '&'))
        return false;
    // Check if the name contains spaces or commas, or ASCII 07 (bell)
    for (std::string::size_type i = 1; i < name.length(); ++i) // skip 0 position == & || #
	{
        if (name[i] == ' ' || name[i] == ',' || name[i] == '\a')
            return false;
    }
    // Name should not be empty after the prefix and max 50 chars
    if (name.length() < 2 || name.length() > 50)
        return false;
    return true;
}

/*	[1] No cal comprovar si un client ja esta connectat a un canal, pq el propi IRSSI ja no envia el missatge join
	al servidor, i el que fa IRSSI es anar a la finestra del canal si repetim l'ordre "/join #samechanname"
	SAME per a "not enough parameters" (just "/join", per exemple), ja ho diu IRSSI
*/
