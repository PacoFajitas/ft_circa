/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:45:10 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/29 16:45:10 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

static bool	isNicknameInUse(const std::string& nickname, const Server& server) {
	const std::map<int, Client*>& clients = server.getClients();
	std::map<int, Client*>::const_iterator it = clients.begin();

	while (it != clients.end()) {
		if (it->second->getNickname() == nickname) {
			return (true);
		}
		++it;
	}
    return (false);
}


// Para validar el nick, primero si son caracteres validos, luego si ya existe
static bool	validateNickname(const std::string& nickname, Client& client, Server& server) {
	if (!server.isValidNickChan(nickname, false)) {
		server.sendResponse(client.getSocketFD(), ERR_ERRONEUSNICKNAME(nickname));
		return (false);
		}
	if (isNicknameInUse(nickname, server)) {
		server.sendResponse(client.getSocketFD(), ERR_NICKNAMEINUSE(nickname));
		return (false);
    }
	return (true);
}


// Para cambiar a otro nick // No hay que  comprobar que el nick existe????
static void	changeNickname(Client& client, const std::string& nickname, Server& server) {
	std::string oldNickname = client.getNickname();
	std::vector<Channel*>vec = server.getChannelsFromClient(client);
	client.setNickname(nickname);
	for (std::vector<Channel*>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Channel* channel = *it;
		std::string clientList = channel->clientOpList();
		channel->sendMessage(RPL_NICKCHANGE(oldNickname, client.getUsername(), client.getHostname(), client.getNickname()), -1);
    	channel->sendMessage(RPL_NAMREPLY(server.getServerName(), client.getNickname(), channel->getName(), clientList), -1);
        channel->sendMessage(RPL_ENDOFNAMES(client.getNickname(),channel->getName()), -1);
	}
	// std::string nickMessage = ":" + oldNickname + " NICK " + client.getNickname(); 
	// server.sendResponse(client.getSocketFD(), RPL_NICKUSER(oldNickname, client.getNickname()));
	server.sendResponse(client.getSocketFD(), RPL_NICKCHANGE(oldNickname, client.getUsername(), client.getHostname(), client.getNickname()));
}

// Funcion registro de cliente, cuando tenemos todos los campos OK
static void	registerClient(Client& client, Server& server) 
{
	client.setRegistered(true);//Ponemos la flag en true
	server.sendResponse(client.getSocketFD(), RPL_WELCOME(client.getNickname(), server.getServerName(), client.getHostname()));
	server.sendResponse(client.getSocketFD(), RPL_YOURHOST(server.getServerName(), client.getNickname()));
	server.sendResponse(client.getSocketFD(), RPL_CREATED(server.getServerName(), server.getTime()));
	server.sendResponse(client.getSocketFD(), RPL_MYINFO(server.getServerName(), client.getNickname()));
}



bool	handleNickCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
	if (tokens.size() < 2) {
		server.sendResponse(client.getSocketFD(), ERR_NONICKNAMEGIVEN());
		return (false);
    }
    std::string nickname = tokens[1];

    // Validar si el nickname contiene caracteres no permitidos
	if (!validateNickname(nickname, client, server)) {
		return (false);
	}
	if (nickname.length() > 9) { // Ajusta el límite según sea necesario
		server.sendResponse(client.getSocketFD(), ERR_ERRONEUSNICKNAME(nickname));
		return (false);
	}

	if (isNicknameInUse(nickname, server)) {
		server.sendResponse(client.getSocketFD(), ERR_NICKNAMEINUSE(nickname));
		return (false);
	}

    // Verificar si el nickname ya está en uso
	if (client.isRegistered()) {
		changeNickname(client, nickname, server);
	}
// Si no está registrado completamente, actualizar el nick y verificar si ya puede
	else {
		client.setNickname(nickname);

		if (client.canBeRegistered()) {
			registerClient(client, server);
		}
	}
	return (true);
}

