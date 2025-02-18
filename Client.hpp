/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:22:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/18 18:23:05 by ccarrace         ###   ########.fr       */
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
		Client & operator=(const Client & source);

		/* --- Getters and Setters ------------------------------------------ */

		std::string & getBuffer();
		
		void		setFd(int fd);
		int			getFd();
		void		setNickname(const std::string & nickname);
		std::string	getNickname();
		void		setUsername(const std::string & username);
		void		setAuthentication(bool _isAuthenticated);
		bool		getAuthentication();
		void		setRegistration(bool _isRegistered);
		bool		getRegistration();

	private:

		int			_fd;
		std::string	_nickname;
		std::string	_username;
		bool		_isAuthenticated;
		bool		_isRegistered;
		std::string	_clientBuffer;

		/* --- Private Coplien's functions ---------------------------------- */



		/* --- Other private methods ---------------------------------------- */
		
};

#endif
