/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:22:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/22 01:51:46 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Server.hpp"

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
	//	int 						getChannelCnt(void) const;
	//	void 						setChannelCnt(void);
	//	int							getMaxChannels(void) const;
		std::map<std::string, bool>	&getChannels();
	
		/* --- Other public methods ----------------------------------------- */
		void			addChannel(std::string &channel, bool isChanOp);
		//	void			_sendMessage(const std::string &message) const;
		bool						unsubscribe(std::string channelName);		
		bool						isOperator(std::string &ch_name);
	
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
	//	int							_maxChannels;
		std::map<std::string, bool> _channels; // key = channel name, bool: whether the client is operator (chanop / admin) of that channel OR NOT
};

#endif
