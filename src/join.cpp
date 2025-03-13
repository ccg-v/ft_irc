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
	{
        keys = splitByComma(msg.parameters[1]);
	}
	int n = chs2join.size();
	for (int i = 0; i < n; i++)
	{
		std::string ch_name = chs2join[i];
		std::string	key = "";
		if (somekey && keys[i] != "x")  //CHECK NETCAT
			key = keys[i];
		else
			key = "";
    	if (client.getSubscriptions().size() >= MAXCHAN)
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

			client.addSubscription(&newChannel, true); //add the channel to the client's channels map: std::map<Channel*, bool> _subscriptions
std::cout << "[DEBUG~]: Client's subscriptions:\t";
for (std::map<Channel*, bool>::iterator it = client.getSubscriptions().begin(); it != client.getSubscriptions().end(); ++it)
{
	std::cout << " " << it->first->getName() << " | isOp = " << it->second << ";";
}
std::cout << std::endl;

			newChannel.addMember(&client); //add client to the Channel clients vector:	std::set<Client*> _members;
std::cout << "[DEBUG~]: Members in " << newChannel.getName() << ":\t";
for (std::set<Client*>::iterator it = newChannel.getMembers().begin(); it != newChannel.getMembers().end(); ++it)
{
	std::cout << " " << (*it)->getNickname();
}
std::cout << std::endl;

			this->_channels[ch_name] = newChannel; //Store the MODIFIED channel to the Server channels map:	std::map<std::string, Channel*>	_channels;
std::cout << "[DEBUG~]: Channels in server:\t ";
for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
{
	std::cout << " " << it->first << "(" << it->second.getName() << ")"; 
}
std::cout << std::endl;
		}
		else  //channel exists
		{
			Channel *channel = _findChannelByName(ch_name);
			if (this->_onChannel(client, *channel)) //NOT SHOWING ON IRSSI (which manages that itself) - CHECK NETCAT
				this->_sendMessage(client, ERR_USERONCHANNEL(this->_serverName, client.getNickname(), ch_name));
		    // TO TEST - NOT WORKING
			else if (channel->getIonly() && _isInvited(*channel, client.getFd())) // 2a CONDICIO: MIRAR si ha rebut una invitacio
                this->_sendMessage(client, ERR_INVITEONLYCHAN(this->_serverName, client.getNickname(), ch_name));
			// PENDING COMPLEX TESTS!!!
			else if (channel->getKey() != "" && key != channel->getKey())
				this->_sendMessage(client, ERR_BADCHANNELKEY(this->_serverName, client.getNickname(), ch_name));
			else if (channel->getLimit() && channel->getMembers().size() == static_cast<size_t>(channel->getLimit()))
				this->_sendMessage(client, ERR_CHANNELISFULL(this->_serverName, client.getNickname(), ch_name));
			//JOIN THE CLIENT TO THE CHANNEL with 'false' as operator: 1. Add the channel to the list of channels for that client, 2. Add the client to the list of clients for the channel 
			else
			{
				client.addSubscription(channel, false); //add the channel to the client's channels map
				channel->addMember(&client); //add client to the Channel clients vector
				// TODO: Send the topic (TOPIC message) and names list (NAMES message) to the joining user.

				std::cout << "[DEBUG~]: Client's subscriptions:\t";
				for (std::map<Channel*, bool>::iterator it = client.getSubscriptions().begin(); it != client.getSubscriptions().end(); ++it)
				{
					std::cout << " " << it->first->getName() << " | isOp = " << it->second << ";";
				}
				std::cout << std::endl;

				std::cout << "[DEBUG~]: Members in " << channel->getName() << ":\t";
				for (std::set<Client*>::iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it)
				{
					std::cout << " " << (*it)->getNickname();
				}
				std::cout << std::endl;
								
			}
		}
	}
}

// TO TEST
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
    // std::map<std::string, Channel>::iterator it;
    // for (it = this->_channels.begin(); it != this->_channels.end(); ++it)
    // {
    //     if (it->first == name)
    //         return (true);
    // }
    // return (false);

    std::map<std::string, Channel>::iterator it = this->_channels.find(name);
    
    if (it == _channels.end())
        return (false);
    
    return (true);
}

bool Server::_validChannelName(std::string &name)
{
    // Check if the name is empty or if it doesn't start with either '#' or '&'
    if (name.empty() || (name[0] != '#' && name[0] != '&'))
        return false;

    // Check if the name contains spaces or commas, or ASCII 07 (bell)
    for (std::string::size_type i = 1; i < name.length(); ++i) // [2]
	{
        if (name[i] == ' ' || name[i] == ',' || name[i] == '\a')
            return false;
    }
    // Name should not be empty after the prefix and max 50 chars
    if (name.length() < 2 || name.length() > 50)
        return false;
    return true;
}

//     std::vector<std::string> channels;
//     std::vector<std::string> keys;
//     Channel channel;
//     std::vector<int> fds;

//     if (msg->parameters.size() < 2)
//     {
//         client._sendMessage(ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN"));
//         return;
//     }
//     bool are_keys = args.size() > 2 ? true : false;
//     channels = _parseMessage(args[1], ',');
//     if (are_keys)
//     {
//         keys = _parseMessage(args[2], ',');
//     }
//     int n = channels.size();
//     for (int i = 0; i < n; i ++)
//     {
//         std::string chan_name = channels[i];
//         if (_channelExist(chan_name))
//         {
//             // check for invite only:
//             if (channel.getMode('i'))
//             {
//                 client._sendMessage(ERR_INVITEONLYCHAN(client.getNickname(), chan_name));
//                 return;
//             }
//             // check if too many channels for client
//             if (client.getChannelCnt() >= client.getChannelLim())
//             {
//                 client._sendMessage(ERR_TOOMANYCHANNELS(client.getNickname(), chan_name));
//                 return;
//             }
//             // check if too many clients in channel
//             if (channel.getClientCnt() >= channel.getClientLim())
//             {
//                 client._sendMessage(ERR_CHANNELISFULL(client.getNickname(), chan_name));
//                 return;
//             }
//             // check the key if it is required for channel
//             if (channel.getMode('k'))
//             {
//                 if (!are_keys || i >= (int)keys.size() || keys[i] != channel.getKey())
//                 {
//                     client._sendMessage(ERR_BADCHANNELKEY(client.getNickname(), chan_name));
//                     return;
//                 }
//             }
// // #ifdef DEBUG
// //             std::cout << "SOCK TO PUSH::" << sock << std::endl;
// // #endif            
//             // this->_channels[chan_name].addClient(sock);
//             this->_channels[chan_name].addClient(sock);
//             this->_clients[sock].addChannel(chan_name, false);
//             fds = this->_channels[chan_name].getClients();
//         }
//         else
//         {
// // #ifdef DEBUG
// //             std::cout << "Channel does not exist, creating a new one\n";
// // #endif
//             if (client.getChannelCnt() >= client.getChannelLim())
//             {
//                 client._sendMessage(ERR_TOOMANYCHANNELS(client.getNickname(), chan_name));
//                 return;
//             }
//             // check for first char and valid chars:
//             if (!_validChannelName(chan_name))
//             {
//                 client._sendMessage(ERR_BADCHANMASK(chan_name));
//                 return;
//             }
//             Channel new_channel(chan_name);
//             new_channel.addClient(sock);
//             if (are_keys && i < (int)keys.size())
//             {
//                 new_channel.setKey(keys[i]);
//                 new_channel.setMode('k', true);
//             }
//             this->_channels[chan_name] = new_channel;
//             this->_clients[sock].addChannel(chan_name, true);
//             fds = new_channel.getClients();
//             channel = new_channel;
//         }
// // #ifdef DEBUG
// //         std::cout << "FDS:: ";
// //         for (size_t i = 0; i < fds.size(); i++)
// //         {
// //             std::cout << fds[i] << ", ";
// //         }
// //         std::cout << std::endl;
// // #endif
//         client._sendMessage(RPL_JOIN(client.getNickname(), chan_name)); // or sendChannel ?
//         if (channel.getMode('t'))
//         {
//             client._sendMessage(RPL_TOPIC(client.getNickname(), chan_name, channel.getTopic()));
//         }
//         // RPL_NAMREPLY, RPL_ENDOFNAMES:
//         _rplNamesList(sock, chan_name, fds);
//         fds.clear();
//     }
// }



// void Server::_rplNamesList(int client, std::string& channel, std::vector<int>& socks)
// {
//     int n = socks.size();
//     std::string nick;
//     std::string prefix = "";

//     std::string client_nick = this->_clients[client].getNickname();
    
//     for (int i = 0; i < n; i ++)
//     {
//         nick = this->_clients[socks[i]].getNickname();
//         // prefix ?? :
//         if (this->_clients[socks[i]].isAdmin(channel))
//         {
//             prefix = "@";
//         }
//         this->_clients[client]._sendMessage(RPL_NAMREPLY(client_nick, channel, prefix, nick));
//         prefix = "";
//     }
//     nick = "TheBot";
//     this->_clients[client]._sendMessage(RPL_NAMREPLY(client_nick, channel, "", nick));
//     this->_clients[client]._sendMessage(RPL_ENDOFNAMES(client_nick, channel));
// }


/*	[1] No cal comprovar si un client ja esta connectat a un canal, pq el propi IRSSI ja no envia el missatge join
	al servidor, i el que fa IRSSI es anar a la finestra del canal si repetim l'ordre "/join #samechanname"
	SAME per a "not enough parameters" (just "/join", per exemple), ja ho diu IRSSI
*/

/*	[2] Loop Variable Declaration
	std::string::size_type i = 1;
*/
