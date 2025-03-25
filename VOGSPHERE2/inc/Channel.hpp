#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <algorithm>	// std::find
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
		Channel(std::string name, std::string key);
		Channel(const Channel &src);
		Channel	&operator=(const Channel &);

		/* -------- SETTERS ------------------*/
		void	setKey(std::string key);
		void	setLimit(int limit);
		void 	setIonly(bool status);
		void 	setTmode(bool status);
		void	setTopic(const std::string & topic);


		/* -------- GETTERS ------------------*/
		std::string			getName(void) const;
		std::string			getTopic(void) const;
		std::string			getKey(void) const;
		int					getLimit(void) const;
		bool 				getIonly() const;
		bool 				getTmode() const;
		std::vector<int>	getClients(void) const;
		std::vector<int>	&getClientsByRef();
		void				setClients(const std::vector<int>& newClients);
		std::vector<int>	&getInvited(void);
		std::string			getTopic();
		void				setTopicAuthor(const std::string & nickname);
		std::string			getWhoSetTopic();

		/* -------- OTHER METHODS ------------------*/
		void				addClient(const int &fd);
		void				addInvite(const int fd);
		void				removeMember(int fd);
		bool				isMember(int fd);

	private:
		std::string			_name;
		std::string 		_topic;
		std::string			_key;
		bool				_ionly;
		bool				_tmode; // restrict topic changes to operators
		int					_limit; //set with the 'l' mode
		std::string			_whoSetTopic;

		std::vector<int>	_clients; // fds of clients
		std::vector<int>	_invited; // fds of clients that have been invited to the channel
};

#endif
