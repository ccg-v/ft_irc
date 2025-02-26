/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 03:23:26 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/25 20:51:44 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
# define REPLIES_HPP

#include "utils.hpp"

/* --- PASS command --------------------------------------------------------- */

#define ERR_PASSWDMISMATCH(serverName, nick) (":" + serverName + " 464 " + nick + \
	" :Password not given or incorrect. Please authenticate first.\r\n")

#define ERR_ALREADYREGISTERED(serverName) (":" + serverName + " 462 " + \
	" :You may not reregister\r\n")

/* --- NICK command --------------------------------------------------------- */

//	ERR_PASSWDMISMATCH()
#define ERR_NEEDMOREPARAMS(serverName, command) (":" + serverName + " 461 " + command + \
	" :Not enough parameters\r\n")

//	ERR_NICKNAMEINUSE()
//	ERR_ERRONEOUSNICKNAME()

/* --- USER command --------------------------------------------------------- */

//	ERR_PASSWDMISMATCH()
//	ERR_NEEDMOREPARAMS()

//	ERR_ALREADYREGISTERED()

// 	?? ERR_INVALIDUSERNAME()

/* --- JOIN command --------------------------------------------------------- */

#define ERR_TOOMANYCHANNELS(serverName, client, channel) (":" + serverName + " 405 " + client + " " + channel + \
	" :Cannot join channel because you already joined 3 channels.\r\n")


/* --- NOTICES -------------------------------------------------------------- */

#define NTC_USERMISSING(serverName, nick) (":" + serverName + " NOTICE" + nick + \
	" :Enter username to complete register\r\n")
#define NTC_NICKMISSING(serverName, user) (":" + serverName + \
	" NOTICE * :Enter nickname to complete register\r\n")
#define NTC_NICKCHANGED(serverName, nick) (":" + serverName + \
	" NOTICE :Nickname changed to " + nick + "\r\n")

/* --- Handshake after complete register ------------------------------------ */

#define RPL_WELCOME(serverName, nick, user, host) (":" + serverName + " 001 " + nick + \
	" :Welcome to the Internet Relay Network, " + nick + "!" + user + "@" + host + "\r\n")
#define RPL_YOURHOST(serverName, nick) (":" + serverName + " 002 " + nick + \
	" :Your host is " + serverName + ", running version 1.0\r\n")
#define RPL_CREATED(serverName, nick) (":" + serverName + " 003 " + nick + \
	" :This server was created on " + this->_creationTime + "\r\n")
#define RPL_MYINFO(serverName, nick) (":" + serverName + " 004 " + nick + " :" + serverName + \
	" 1.0 -availableusermodes- -availablechannelmodes-\r\n")

#endif
