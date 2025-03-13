/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:22:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/12 20:00:01 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Server.hpp"
#include "Channel.hpp"

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
		int 						getChannelCnt();
		// void 						setChannelCnt(void);
	//	int							getMaxChannels(void) const;
		std::map<Channel*, bool>	&getSubscriptions();
	
		/* --- Other public methods ----------------------------------------- */
		// void			addChannel(std::string &channel, bool isChanOp);
		//	void			_sendMessage(const std::string &message) const;
		void 						addSubscription(Channel *channel, bool isChanOp);
		void						unsubscribe(Channel &channel);
	
	private:

		int							_fd;
		std::string					_clientIp;
		std::string					_nickname;
		std::string					_username;
		std::string					_realname;
		std::string					_hostMask;
		bool						_isAuthenticated;
		bool						_isRegistered;
		std::string					_clientBuffer;
	//	int							_maxChannels;
		// std::map<std::string, bool> _channels; // key = channel name, bool: whether the client is operator (chanop / admin) of that channel OR NOT
		std::map<Channel*, bool>	_subscriptions;
};

#endif
