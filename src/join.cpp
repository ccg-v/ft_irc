#include "Server.hpp"

void Server::_join(Client &client, const t_tokens msg) //[1]
{
	std::vector<std::string>	chs2join; // in case there are several channels to join
    std::vector<std::string>	keys;	// if there are several channels with several keys
//	Channel						channel;
//	std::cout << "[~DEBUG]: STARTING JOIN FT - parameter 0 (channel name) is: " << msg.parameters[0] << std::endl;
	bool somekey = msg.parameters.size() > 1 ? true : false; //true if >1 parameters; means client sent some key to some channel
    if (msg.parameters.size() < 1)
		return ;
	chs2join = _splitByComma(msg.parameters[0]);
    if (somekey)
	{
        keys = _splitByComma(msg.parameters[1]);
	}
	int n = chs2join.size();
	for (int i = 0; i < n; i++)
	{
		std::string ch_name = chs2join[i];
		// std::cout << "[DEBUG]: chs2join[" << i << "]: " << chs2join[i] << std::endl;
		// std::cout << "[DEBUG]: ch_name: " << ch_name << std::endl;
		//std::cout << "[DEBUG]: channel i mode: " << channel.getMode('i') << std::endl;

		// check if too many channels for client
    	if (client.getChannels().size() >= MAXCHAN)
		{
			this->_sendMessage(client, ERR_TOOMANYCHANNELS(this->_serverName, client.getNickname(), ch_name));
        	return;
    	}
		// check if channel doesn't exist yet
		if (!this->_chanExists(ch_name)) 
		{
				// check valid channel name
				if (!this->_validChannelName(ch_name))
				{
					this->_sendMessage(client, ERR_BADCHANMASK(this->_serverName, ch_name));
					return;
				}
				Channel newChannel(ch_name); //instantiate the channel
				client.addChannel(ch_name, true); //add the channel to the clint's channels map
				this->_channels[ch_name] = newChannel; //add the channel to the Server channels map
				newChannel.addClient(client.getFd()); //add client to the Channel clients vector
	// #ifndef DEBUG
	// 		std::cout << "[~DEBUG]: channel MODE i: " << newChannel.getMode('i') << std::endl;
	// 		std::cout << "[~DEBUG]: channel MODE t: " << newChannel.getMode('t') << std::endl;
	// 		std::cout << "[~DEBUG]: channel MODE k: " << newChannel.getMode('k') << std::endl;
	// 		std::cout << "[~DEBUG]: channel MODE o: " << newChannel.getMode('o') << std::endl;
	// 		std::cout << "[~DEBUG]: channel MODE l: " << newChannel.getMode('l') << std::endl;
	// #endif
		}
		else  //channel exists
		{
			Channel &channel = this->_channels[ch_name];
			//descomentar tot el de sota un cop gestionats els MODES
			if (channel.getMode('i'))  // CAL MIRAR tb si ha rebut una invitacio (IDEA MESTRA)
            {
				//std::cout << "[DEBUG]: entres IF (channel.getMode(i))" << std::endl;
                this->_sendMessage(client, ERR_INVITEONLYCHAN(this->_serverName, ch_name));
                return;
            }
			//JOIN THE CLIENT TO THE CHANNEL with 'false' as operator: 1. Channel, 2. List of channels for that client
		}
	}
}

std::vector<std::string>	Server::_splitByComma(const std::string &str)
{
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, ','))   // Extract until ','
	{  
        result.push_back(token);
    }
    return result;
}

bool	Server::_chanExists(const std::string &name)
{  
	//std::cout << "[~DEBUG] chanExists ft: first channel name: " << this->_channels.begin()->first << std::endl;
    std::map<std::string, Channel>::iterator it;
    for (it = this->_channels.begin(); it != this->_channels.end(); it++)
    {
        if (it->first == name)
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
