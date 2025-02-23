/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 03:23:26 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/23 22:29:06 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
# define REPLIES_HPP

#include "utils.hpp"

// 464	ERR_PASSWDMISMATCH	<client> :<reason> 				Returned by the PASS command to indicate the given password was required and was either not given or was incorrect 
// 461 	ERR_NEEDMOREPARAMS	<client> <command> :<reason>	Returned by the server by any command which requires more parameters than the number of parameters given 

// Early handshake
#define RPL_WELCOME(serverName, nick, user, host) (":" + serverName + " 001 " + nick + \
	" :Welcome to the Internet Relay Network, " + nick + "!" + user + "@" + host + "\r\n")
#define RPL_YOURHOST(serverName, nick) (":" + serverName + " 002 " + nick + \
	" :Your host is " + serverName + ", running version 1.0\r\n")
#define RPL_CREATED(serverName, nick) (":" + serverName + " 003 " + nick + \
	" :This server was created " + this->_creationTime + "\r\n")
#define RPL_MYINFO(serverName, nick) (":" + serverName + " 004 " + nick + " :" + serverName + \
	" 1.0 -availableusermodes- -availablechannelmodes-\r\n")

// Pass
// #define ERR_PASSWDMISMATCH(nick, user) "001 " + nick + " :Welcome to the Internet Relay Network, " + user

// Nick

// User
#define ERR_NEEDMOREPARAMS(serverName, command) (":" + serverName + " 461 " + command + \
	" :Not enough parameters\r\n")


#endif
