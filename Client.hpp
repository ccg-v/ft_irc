/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 16:22:49 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/14 22:15:35 by ccarrace         ###   ########.fr       */
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

		/* --- Other public methods ----------------------------------------- */

		std::string & getBuffer();
		
// void setBuffer(std::string buffer);
// void addBuffer(std::string & buffer);

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