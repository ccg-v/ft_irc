#include "Server.hpp"

void Server::_invite(Client &client, const t_tokens msg)
{
	std::string ch_name = toLowerStr(msg.parameters[1]);
	const t_tokens *ms = &msg;
	if (ms->parameters.size() < 2) // check syntax
	{
		this->_sendMessage(client, ERR_NEEDMOREPARAMS(this->_serverName, ms->parameters[0]));
		return ;
	}
	std::string &server = this->_serverName;
	const std::string &targetnick = msg.parameters[0];
	const std::string &nick = client.getNickname();
	//0. CHECK IF the invited client exists
	if (!_nickExists(targetnick))
	{
		_sendMessage(client, ERR_NOSUCHNICK(server, nick, targetnick));
		return ;
	}
	//1. CHECK IF the channel exists
	if (!_chanExists(ch_name)) //channel does not exist
	{
		this->_sendMessage(client, ERR_NOSUCHCHANNEL(this->_serverName, client.getNickname(), msg.parameters[0]));
		return ;
	}
	Channel *channel = _getChannel(ch_name);
	//2. CHECK IF inviter is on that channel
	if (!_onChannel(client, ch_name))
	{
		this->_sendMessage(client, ERR_NOTONCHANNEL(server, nick, ch_name));
     	return ;
	}
	//3. CHECK IF inviter has permission to invite to the channel
	std::map<std::string, bool>::iterator it = client.getChannels().find(ch_name);
	if (!it->second) 
	{
    	this->_sendMessage(client, ERR_CHANOPRIVSNEEDED(server, nick, ch_name));
    	return ;
	}
	//4. CHECK IF the invited client is already on the channel
	Client *targetCl = NULL;
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
        if (it->second.getNickname() == targetnick)
			targetCl = &(it->second);
    }
	if (targetCl && _onChannel(*targetCl, ch_name))
	{
		this->_sendMessage(*targetCl, ERR_USERONCHANNEL(server, targetnick, ch_name));
     	return ;
	}
	//5. CHECK if the channel is invite-only (otherwise the server should do nothing)
	if (targetCl && channel->getIonly() == true)
	{
		channel->addInvite(targetCl->getFd());
		this->_sendMessage(*targetCl, NTC_INVITATIONRECEIVED(server, nick, targetnick, ch_name));
		this->_sendMessage(client, NTC_INVITATIONSUCCESS(server, nick, targetnick, ch_name));
	}
}
