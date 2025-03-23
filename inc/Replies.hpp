/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 03:23:26 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/23 19:51:39 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
# define REPLIES_HPP

#include "utils.hpp"

/* --- PASS command --------------------------------------------------------- */
#define ERR_PASSWDMISMATCH(serverName, nick) \
	(":" + serverName + " 464 " + nick + " :Password not given or incorrect.\r\n")
#define ERR_ALREADYREGISTERED(serverName) \
	(":" + serverName + " 462 " + " :You may not reregister\r\n")

/* --- NICK command --------------------------------------------------------- */
//	ERR_PASSWDMISMATCH()
#define ERR_NONICKNAMEGIVEN(serverName) \
	(":" + serverName + " 431" + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(serverName, nick) \
	(":" + serverName + " 432 " + nick + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(serverName, nick, newNick) \
    (":" + serverName + " 433 " + nick + " " + newNick + " :Nickname is already in use\r\n")

/* --- USER command --------------------------------------------------------- */
//	ERR_PASSWDMISMATCH()
//	ERR_ALREADYREGISTERED()
#define ERR_NEEDMOREPARAMS(serverName, command) \
	(":" + serverName + " 461 " + command + " :Not enough parameters\r\n")
#define ERR_INVALIDUSERNAME(serverName, user) \
	(":" + serverName + " " + user + " :Erroneus username\r\n")

/* --- JOIN command --------------------------------------------------------- */
#define ERR_TOOMANYCHANNELS(serverName, client, channel) \
	(":" + serverName + " 405 " + client + " " + channel + \
	" :You have joined too many channels\r\n")
#define ERR_BADCHANMASK(serverName, chan_name) \
	(":" + serverName + " 476 " + chan_name + " :Bad Channel Mask.\r\n")
#define ERR_INVITEONLYCHAN(serverName, nickname, channel) \
	(":" + serverName + " 473 " + nickname + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(serverName, nickname, channel) \
	(":" + serverName + " 475 " + nickname + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_USERONCHANNEL(serverName, nick, channel) \
	(":" + serverName + " 443 " + channel + " " + nick + " :is already on channel\r\n")
#define ERR_CHANNELISFULL(serverName, nick, channel) \
	(":" + serverName + " 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_KEYSET(serverName, nick, channel) \
	(":" + serverName + " 467 " + nick + " " + channel + " :Channel key must be 2-32 chars long and contain only alphanumeric chars or $%&'()*+-.@\r\n")
//#define RPL_NAMREPLY(serverName, joinernick, channel, prefix, membernick) 
//	(":" + serverName + " 353 " + joinernick + " = " + channel + " :" + prefix + membernick + "\r\n")
// #define RPL_NAMREPLY(serverName, joinernick, channel, namesList) 
// 	(":" + serverName + " 353 " + joinernick + " = " + channel + " :" + namesList + "\r\n")
// #define RPL_ENDOFNAMES(serverName, client, channel) 
// 	(":" + serverName + " 366 " + client + " " + channel + " :End of /NAMES list\r\n")

/* --- MODE command --------------------------------------------------------- */
#define ERR_NOTONCHANNEL(serverName, client, channel) \
	(":" + serverName + " 442 " + client + " " + channel + " :You're not on that channel\r\n")
#define ERR_INVALIDMODEPARAM(serverName, clientNick, mode, parameter) \
    (":" + serverName + " 696 " + clientNick + " " + mode + " " + parameter + " :Invalid limit\r\n")
#define ERR_CHANNELLIMITTOOLOW(serverName, clientNick, limit) \
    (":" + serverName + " 476 " + clientNick + " " + limit + " :Limit too low, users would be kicked\r\n")
#define ERR_CHANOPRIVSNEEDED(serverName, client, channel) \
	(":" + serverName + " 482 " + client + " " + channel + " :You're not channel operator\r\n")
#define ERR_TOOMANYPARAMS(serverName, command) \
	(":" + serverName + " 461 " + command + " :Too many parameters\r\n")
#define ERR_MODEERROR(serverName, command, mode) \
	(":" + serverName + " " + command + " " + mode + " :Available modes: +k -k +i -i +o -o +t -t +l -l\r\n")

/* --- KICK command --------------------------------------------------------- */
//	ERR_NEEDMOREPARAMS()
//	ERR_CHANOPRIVSNEEDED()
//	ERR_NOTONCHANNEL()
#define ERR_NOSUCHCHANNEL(serverName, client, channel) \
	(":" + serverName + " 403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_NOSUCHNICK(serverName, client, nick) \
	(":" + serverName + " 401 " + client + " " + nick + " :No such nick\r\n")
#define ERR_USERNOTINCHANNEL(serverName, client, channel) \
	(":" + serverName + " 441 " + client + " " + channel + " :Not on that channel\r\n")

/* --- TOPIC command -------------------------------------------------------- */
//	ERR_NEEDMOREPARAMS()
//	ERR_NOSUCHCHANNEL()
//	ERR_NOTONCHANNEL()
//	ERR_CHANOPRIVSNEEDED()
#define RPL_NOTOPIC(serverName, client, channel) \
	(":" + serverName + " 331 " + client + " " + channel + " :No topic is set\r\n")
#define RPL_TOPIC(serverName, client, channel, topic) \
	(":" + serverName + " 332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_TOPICWHOTIME(serverName, client, channel, whoSetNick, wasSetAt) \
	(":" + serverName + " 333 " + client + " " + channel + " " + whoSetNick + " " + wasSetAt + "\r\n")

/* --- PRIVMSG command ------------------------------------------------------ */
//	ERR_NEEDMOREPARAMS()
//	ERR_USERNOTINCHANNEL()
//	ERR_NOSUCHNICK()
//	ERR_NOSUCHCHANNEL()
#define ERR_NOTREGISTERED(serverName, client) \
	(":" + serverName + " 451 " + client + " :You have not registered\r\n")
#define ERR_NOTEXTTOSEND(serverName, client) \
	(":" + serverName + " 412 " + client + " :No text to send\r\n")
#define ERR_INPUTTOOLONG(serverName, client) \
	(":" + serverName + " 417 " + client + " :Input line was too long\r\n")

// #define ERR_NORECIPIENT(serverName, client, command)
// 	(":" + serverName + " 411 " " :No recipient given (" + command + ")\r\n")

/* --- PART command -----------------------------------------------'''------- */
//	ERR_NEEDMOREPARAMS()
//	ERR_NOSUCHCHANNEL()
//	ERR_NOTONCHANNEL()
	
/* --- NOTICES -------------------------------------------------------------- */
#define NTC_USERMISSING(serverName, nick) \
	(":" + serverName + " NOTICE" + nick + " :Enter username to complete register\r\n")
#define NTC_NICKMISSING(serverName, user) \
	(":" + serverName + " NOTICE * :Enter nickname to complete register\r\n")
#define NTC_SERVERSHUTDOWN(serverName, nick) \
	(":" + serverName + " NOTICE * :Server shutting down\r\n")

/* --- Custom messages------------------------------------------------------- */	
#define INF_NICKCHANGED(oldNick, user, clientIp, newNick) \
	(":" + oldNick + "!" + user + "@" + clientIp + " NICK :" + newNick + "\r\n")
#define INF_CLIENTQUIT(serverName, nick) \
	(":" + serverName + " :" + nick + " QUIT :Leaving the server\r\n")
#define INF_KICKEDFROMCHANNEL(serverName, command, channel, nick) \
	(":" + serverName + " :" + command + " " + channel + " " + nick + " :Kicked from channel\r\n")
#define INF_NICKNAMEIS(serverName, nick) \
	(":" + serverName + ": Your nickname is " + nick + "\r\n")
#define INF_SERVERISFULL(serverName) \
	(":" + serverName + ": The server is full. Please try again later \r\n")

/* --- Handshake after complete register ------------------------------------ */
#define RPL_WELCOME(serverName, nick, hostmask) (":" + serverName + " 001 " + nick + \
	" :Welcome to the Internet Relay Network, " + hostmask + "\r\n")
#define RPL_YOURHOST(serverName, nick) (":" + serverName + " 002 " + nick + \
	" :Your host is " + serverName + ", running version 1.0\r\n")
#define RPL_CREATED(serverName, nick) (":" + serverName + " 003 " + nick + \
	" :This server was created on " + this->_creationTime + "\r\n")
#define RPL_MYINFO(serverName, nick) (":" + serverName + " 004 " + nick + " :" + serverName + \
	" 1.0 -availableusermodes- -availablechannelmodes-\r\n")

/* --- _processMessage() ----------------------------------------------------- */
#define ERR_UNKNOWNCOMMAND(serverName, command) \
	(":" + serverName + " 421 " + command + " :Unknown command\r\n")

#endif
