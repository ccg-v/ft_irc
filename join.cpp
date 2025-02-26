#include "Server.hpp"

void Server::_join(Client &client, t_tokens msg) //[1]
{
	std::vector<std::string> channels; // in case there are several channels to join
    std::vector<std::string> keys;	// if there are several channels with several keys

	bool somekey = msg.parameters.size() > 1 ? true : false; //true if >1 parameters; means client sent some key to some channel
    if (msg.parameters.size() < 1)
		return ;
	channels = _splitByComma(msg.parameters[0]);
    if (somekey)
	{
        keys = _splitByComma(msg.parameters[1]);
	}
	int n = channels.size();
	for (int i = 0; i < n; i++)
	{
		std::string name = channels[i];
		// check if too many channels for client
    	if (client.getChannelCnt() >= client.getMaxChannels())
		{
			this->sendMessage(client, ERR_TOOMANYCHANNELS(this->_serverName, client.getNickname(), name));
        	return;
    	}
		if (this->_chanExists(name))  // check if channel already exists
		{
			std::cout << "[~DEBUG]: channel exists" << std::endl;
			//JOIN THE CLIENT TO THE CHANNEL with 'false' as operator: 1. Channel, 2. List of channels for that client
		}
		else  //channel creation
		{
			// check valid channel name
			if (!this->_validChannelName(name))
			{
				std::cout << "[~DEBUG]: not valid channel name" << std::endl;
				//[TODO] send error
				return ;
			}
			client.addChannel(msg.parameters[0], true);
			Channel newChannel(msg.parameters[0]); //instantiate the channel
			this->_channels[msg.parameters[0]] = newChannel; //add the channel to the Server channels map
		
		// std::cout << "[~DEBUG]: client current channels: " << std::endl;
		// for (std::map<std::string, bool>::const_iterator it = client.getChannels().begin(); it != client.getChannels().end(); ++it)
		// {
    	// 	std::cout << it->first << std::endl;
		// }
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

bool	Server::_chanExists(std::string &name)
{  
	//std::cout << "[~DEBUG] chanExists ft: first channel name: " << this->_channels.begin()->first << std::endl;
    std::map<std::string, Channel>::iterator it;
    for (it = this->_channels.begin(); it != this->_channels.end(); it++)
    {
        if (it->first == name)
		{
			std::cout << "[~DEBUG] chanExists ft: CHANNEL ALREADY EXISTS" << std::endl;
            return (true);
		}
    }
	std::cout << "[~DEBUG] chanExists ft: channel does not exist" << std::endl;
    return (false);
}

//     std::vector<std::string> channels;
//     std::vector<std::string> keys;
//     Channel channel;
//     std::vector<int> fds;

//     if (msg->parameters.size() < 2)
//     {
//         client.sendMessage(ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN"));
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
//                 client.sendMessage(ERR_INVITEONLYCHAN(client.getNickname(), chan_name));
//                 return;
//             }
//             // check if too many channels for client
//             if (client.getChannelCnt() >= client.getChannelLim())
//             {
//                 client.sendMessage(ERR_TOOMANYCHANNELS(client.getNickname(), chan_name));
//                 return;
//             }
//             // check if too many clients in channel
//             if (channel.getClientCnt() >= channel.getClientLim())
//             {
//                 client.sendMessage(ERR_CHANNELISFULL(client.getNickname(), chan_name));
//                 return;
//             }
//             // check the key if it is required for channel
//             if (channel.getMode('k'))
//             {
//                 if (!are_keys || i >= (int)keys.size() || keys[i] != channel.getKey())
//                 {
//                     client.sendMessage(ERR_BADCHANNELKEY(client.getNickname(), chan_name));
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
//                 client.sendMessage(ERR_TOOMANYCHANNELS(client.getNickname(), chan_name));
//                 return;
//             }
//             // check for first char and valid chars:
//             if (!_validChannelName(chan_name))
//             {
//                 client.sendMessage(ERR_BADCHANMASK(chan_name));
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
//         client.sendMessage(RPL_JOIN(client.getNickname(), chan_name)); // or sendChannel ?
//         if (channel.getMode('t'))
//         {
//             client.sendMessage(RPL_TOPIC(client.getNickname(), chan_name, channel.getTopic()));
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
//         this->_clients[client].sendMessage(RPL_NAMREPLY(client_nick, channel, prefix, nick));
//         prefix = "";
//     }
//     nick = "TheBot";
//     this->_clients[client].sendMessage(RPL_NAMREPLY(client_nick, channel, "", nick));
//     this->_clients[client].sendMessage(RPL_ENDOFNAMES(client_nick, channel));
// }

bool Server::_validChannelName(std::string &name)
{
    // Check if the name is empty or if it doesn't start with either '#' or '&'
    if (name.empty() || (name[0] != '#' && name[0] != '&'))
        return false;

    // Check if the name contains spaces or commas, or ASCII 07 (bell)
    for (std::string::size_type i = 1; i < name.length(); ++i)
	{
        if (name[i] == ' ' || name[i] == ',' || name[i] == '\a')
            return false;
    }

    // Name should not be empty after the prefix
    if (name.length() < 2)
        return false;

    return true;
}



/*	[1] No cal comprovar si un client ja esta connectat a un canal, pq el propi IRSSI ja no envia el missatge join
	al servidor, i el que fa IRSSI es anar a la finestra del canal si repetim l'ordre "/join #samechanname"
	SAME per a "not enough parameters" (just "/join", per exemple), ja ho diu IRSSI
*/
