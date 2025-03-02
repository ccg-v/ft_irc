/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 03:23:26 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/02 19:22:42 by ccarrace         ###   ########.fr       */
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

#define ERR_NOSUCHCHANNEL(serverName, client, channel) \
	(":" + serverName + " 403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_TOOMANYCHANNELS(serverName, client, channel) \
	(":" + serverName + " 405 " + client + " " + channel + \
	" :Cannot join channel because you already joined 3 channels.\r\n")

/* --- PRIVMSG command ------------------------------------------------------ */

//	ERR_ERRONEUSNICKNAME()
#define ERR_NOTREGISTERED(serverName, client) \
	(":" + serverName + " 451 " + client + " :You have not registered\r\n")
#define ERR_NOSUCHNICK(serverName, client, nick) \
	(":" + serverName + " 401 " + client + " :No such nick/channel\r\n")
#define ERR_NOTEXTTOSEND(serverName, client) \
	(":" + serverName + " 412 " + client + " :No text to send\r\n")
#define ERR_NORECIPIENT(serverName, client, command) \
	(":" + serverName + " 411 " " :No recipient given (" + command + ")\r\n")

/* --- NOTICES -------------------------------------------------------------- */

#define NTC_USERMISSING(serverName, nick) \
	(":" + serverName + " NOTICE" + nick + " :Enter username to complete register\r\n")
#define NTC_NICKMISSING(serverName, user) \
	(":" + serverName + " NOTICE * :Enter nickname to complete register\r\n")
	
#define INF_NICKCHANGED(oldNick, user, clientIp, newNick) \
	(":" + oldNick + "!" + user + "@" + clientIp + " NICK :" + newNick + "\r\n")

/* --- Handshake after complete register ------------------------------------ */

#define RPL_WELCOME(serverName, nick, user, host) (":" + serverName + " 001 " + nick + \
	" :Welcome to the Internet Relay Network, " + nick + "!" + user + "@" + host + "\r\n")
#define RPL_YOURHOST(serverName, nick) (":" + serverName + " 002 " + nick + \
	" :Your host is " + serverName + ", running version 1.0\r\n")
#define RPL_CREATED(serverName, nick) (":" + serverName + " 003 " + nick + \
	" :This server was created on " + this->_creationTime + "\r\n")
#define RPL_MYINFO(serverName, nick) (":" + serverName + " 004 " + nick + " :" + serverName + \
	" 1.0 -availableusermodes- -availablechannelmodes-\r\n")

/* --- processMessage() ----------------------------------------------------- */

#define ERR_UNKNOWNCOMMAND(serverName, command) \
	(":" + serverName + " 421 " + command + " :Unknown command\r\n")

#endif
