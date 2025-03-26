/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:22:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/26 19:07:48 by erosas-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Server.hpp"

#define CONST = 0;
class	Client
{
	public:

		/* --- Public Coplien's functions ----------------------------------- */

		Client();		// Default constructor
		Client(int fd);	// Parameterized constructor
		~Client();		// Default destructor
		Client(const Client & source);
		Client &operator=(const Client & source);

		/* --- Getters and Setters ------------------------------------------ */

		std::string 				&getBuffer();
		void						setFd(int fd);
		int							getFd();
		void						setClientIp(std::string clientIp);
		std::string					getClientIp();
		void						setNickname(const std::string & nickname);
		std::string					getNickname();
		void						setUsername(const std::string & username);
		std::string					getUsername();
		void						setRealname(const std::string & realname);
		std::string					getRealname();	
		void						setHostMask();
		std::string					getHostMask();
		void						setAuthentication(bool isAuthenticated);
		bool						getAuthentication();
		void						setRegistration(bool isRegistered);
		bool						getRegistration();
		void						setPassErrSent(bool passErrSent);
		bool						getPassErrSent();
		time_t						getStartTime();
		std::map<std::string, bool>	&getChannels();
		void						setNickValid(bool status);
		bool						getNickValid();
	
		/* --- Other public methods ----------------------------------------- */
		void						addChannel(std::string &channel, bool isChanOp);
		void						unsubscribe(std::string channelName);
		bool						isSubscriber(const std::string &channelName);	
		bool						isOperator(const std::string &ch_name);
	
	private:

		int							_fd;
		std::string					_clientIp;
		std::string					_nickname;
		std::string					_username;
		std::string					_realname;
		std::string					_hostMask;
		bool						_isAuthenticated;
		bool						_isRegistered;
		bool						_passErrSent;
		std::string					_clientBuffer;
		std::string					_topic;
		time_t						_startTime;
		std::map<std::string, bool> _channels; // key = channel name, bool: whether the client is operator (chanop / admin) of that channel OR NOT
		bool						_nickValid;
};

#endif
