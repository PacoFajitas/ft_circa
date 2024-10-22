#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "client.hpp"
#include "responses.hpp"

// INCHANNEL
// ALL

enum UserRole { PARTICIPANT, OPERATOR, INVITED };

class Channel {

	public :
		// Constructor
		Channel(const std::string& channel_name, Server& srv, Client& client);

		// GETERS AND SETERS

		const	std::string& getName() const;
		const	bool& getMode(char mode) const;
		const std::map<Client *, UserRole> getUsers() const;
		std::vector<Client *>getUsersWithRole(std::string mode);
		std::set<int>getClientFDs() const;
		const	std::string& getTopic() const;
		void	setTopic(const std::string& newTopic);
		void	setRole(std::string nick,  UserRole rol);
		bool	isUserRole(const Client& client, std::string);
		const	std::string	getAllModes() const; 
		int	getLimitUsers() const; 
		const	std::string	getPassword() const;
		void    setMode(char mode, bool active);
		void    setMode(char mode, bool active, std::string msg);
		Client	*getUser(std::string nick);



		// METHODS

		bool	isUserInChannel(std::string nick);
		void	manageUser(Client* client, UserRole role, bool add);
		void	dummyUseServer();
		bool	hasSentModeToClient(const Client& client);
		void	setModeSentToClient(const Client& client);
		void	sendMessage(const std::string& message, const int _fd);
		std::string clientOpList();


	private:
		std::string name;
		std::string topic;
		std::map<Client*, UserRole> users;
		std::map<char, bool> modes;
		int	limitUsers;
		std::string password;
		Server& server;
		std::set<int> clientsWithModeSent;


};

#endif // CHANNEL_HPP