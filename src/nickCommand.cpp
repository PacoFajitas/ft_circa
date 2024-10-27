// nickCommand.cpp
#include "commands.hpp"
#include "channel.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "utils.hpp"


// Función auxiliar para validar si el nickname contiene caracteres válidos
static	bool isInvalidNick(const std::string& nick) {
	std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-[]\\`^{}|_";
	return (nick.empty() || nick.find_first_not_of(validChars) != std::string::npos);
}


bool	isNicknameInUse(const std::string& nickname, const Server& server) {
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
bool	validateNickname(const std::string& nickname, Client& client, Server& server) {
	if (isInvalidNick(nickname)) {
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
void	changeNickname(Client& client, const std::string& nickname, Server& server) {
	std::string oldNickname = client.getNickname();
	std::vector<Channel*>vec = server.getChannelsFromClient(client);
	client.setNickname(nickname);
	for ( std::vector<Channel*>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Channel* temp = *it;
		temp->sendMessage(RPL_NICKCHANGE(oldNickname, client.getUsername(), client.getHostname(), client.getNickname()), client.getSocketFD());
		std::string clientList;
    	const std::vector<Client*>& participants = temp->getUsersWithRole("INCHANNEL");
		printClientVector(participants);
    	for (size_t i = 0; i < participants.size(); ++i) {
        if (temp->isUserRole(*participants[i], "OPERATOR"))
            clientList += "@";
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    	}

		// Enviar la lista de usuarios (NAMES) solo al cliente que acaba de unirse
    	server.sendResponse(client.getSocketFD(), RPL_NAMREPLY(server.getServerName(), client.getNickname(), temp->getName(), clientList));
		temp->sendMessage(RPL_ENDOFNAMES(client.getNickname(),temp->getName()), -1);
	}
	
	// exclude_fds.insert(client.getSocketFD());
	// server.broadcastMessage(message, std::set<int>(), exclude_fds);
	//server.broadcastMessage(message, std::set<int>(), std::set<int>{client.getSocketFD()});

}

// Funcion registro de cliente, cuando tenemos todos los campos OK
void	registerClient(Client& client, Server& server) {
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

