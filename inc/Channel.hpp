#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"

/* 	Channel names are strings (beginning with a '&' or '#' character) of length up to 200 characters. 
	Apart from the  requirement that the first character being either '&' or '#'; the only restriction on
	a channel name is that it may not contain any spaces (' '), a control G (^G or ASCII 7),
	or a comma (',') which is used as a list item separator by the protocol.
*/

class Client;

class Channel
{
	public:
		Channel(void);
		Channel(std::string name);
		Channel(const Channel &src);
		Channel	&operator=(const Channel &);

		/* -------- SETTERS ------------------*/
		//void setName(std::string name); //VOLS DIR QUE TE SENTIT? NO ES POT CREAR SI NO ES AMB EL NOM
		//  void setTopic(std::string topic);
		void	setKey(std::string key);
		void	setMode(char mode, bool status); // to use only after checking that mode exists!
		void	setLimit(int limit);

		/* -------- GETTERS ------------------*/
		std::string			getName(void) const;
		//std::string		getTopic(void) const;
		std::string			getKey(void) const;
		bool				getMode(char mode) const; // to use only after checking that mode exists!  - erosas: ????
		std::vector<int>	getClients(void) const;

		void				addClient(const int &fd);
		//void _sendMessage(const std::string& message) const;

	private:
		std::string				_name;
		//std::string _topic;
		std::string				_key;
		std::map<char, bool>	_modes; // key is one of the modes: i, t, k, o, l
		std::vector<int>		_clients; // fds of clients
		int						_limit; //set with the 'l' mode
};

#endif
