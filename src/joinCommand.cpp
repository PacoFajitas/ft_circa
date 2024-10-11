#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "responses.hpp"
#include "utils.hpp"
#include <iostream>

/*********************************************************
 * *******************************************************
 * El orden en el código que funciona es:
JOIN
TOPIC
NAMES
MODE
WHO
**********************************************************
*********************************************************/
/*
void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("JOIN"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        channel = new Channel(channelName, server); // Crear un nuevo canal
        server.addChannel(channel);
    }
	std::cout << "Hostname for client: " << client.getHostname() << std::endl;
	std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN :" + channelName;


    channel->manageUser(&client, PARTICIPANT, true);
	// Enviar el mensaje de JOIN al propio client
	server.sendResponse(client.getSocketFD(), joinMsg);

    // Notificar a otros miembros del canal que alguien ha entrado
	std::set<int> exclude_fds;
    exclude_fds.insert(client.getSocketFD());
//    server.broadcastMessage(joinMsg, std::set<int>(), exclude_fds);

	// Enviar a los otros clientes en el canal
	server.broadcastMessage(joinMsg, channel->getClientFDs(), exclude_fds);

    // Enviar el tópico del canal
    if (!channel->getTopic().empty()) {
        server.sendResponse(client.getSocketFD(), RPL_TOPIC(server.getServerName(), channelName, channel->getTopic()));
    } else {
        server.sendResponse(client.getSocketFD(), RPL_NOTOPIC(server.getServerName(), channelName));
    }

    // Preparar y enviar la lista de nombres de los miembros del canal
    std::string clientList;
    const std::vector<Client*>& participants = channel->getParticipants();
    for (size_t i = 0; i < participants.size(); ++i) {
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    }

    server.sendResponse(client.getSocketFD(), RPL_NAMREPLY(server.getServerName(), client.getNickname(), channelName, clientList));
	server.sendResponse(client.getSocketFD(), RPL_ENDOFNAMES(client.getNickname(), channelName));
	std::string mode = "+nt"; // Ajusta según el modo real del canal
	server.sendResponse(client.getSocketFD(), RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), channelName, mode));

    std::cout << client.getNickname() << " joined channel " << channelName << std::endl;
}

*****************/

/*
void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("JOIN"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        // Crear un nuevo canal si no existe
        channel = new Channel(channelName, server);
        server.addChannel(channel);
    }

    // Mensaje de confirmación del JOIN para todos
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + 
                          "@" + client.getHostname() + " JOIN :" + channelName;

    // Enviar el mensaje de JOIN al cliente que se unió
    server.sendResponse(client.getSocketFD(), joinMsg);

    // Añadir el cliente al canal como participante
    channel->manageUser(&client, PARTICIPANT, true);

    // Notificar a los demás miembros del canal sobre el nuevo usuario
    std::set<int> exclude_fds;
    exclude_fds.insert(client.getSocketFD());
    server.broadcastMessage(joinMsg, channel->getClientFDs(), exclude_fds);

    // Enviar la lista completa de mensajes (tópico, lista de usuarios, modos) a todos los usuarios del canal
    const std::vector<Client*>& participants = channel->getParticipants();
    std::string clientList;
    for (size_t i = 0; i < participants.size(); ++i) {
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    }

    // Mensajes que deben enviarse a todos los clientes en el canal
    std::string notopicMsg = ":" + server.getServerName() + " 331 " + channelName + " :No topic is set";
    std::string namesReplyMsg = ":" + server.getServerName() + " 353 " + client.getNickname() + " = " + channelName + " :" + clientList;
    std::string endOfNamesMsg = ":" + server.getServerName() + " 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list";
    std::string modeMsg = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt"; // Ajusta los modos según sea necesario

    // Enviar los mensajes a todos los clientes en el canal sin excluir a nadie
    std::set<int> empty_exclude_fds;  // Un conjunto vacío para no excluir a nadie
    server.broadcastMessage(notopicMsg, channel->getClientFDs(), empty_exclude_fds);
    server.broadcastMessage(namesReplyMsg, channel->getClientFDs(), empty_exclude_fds);
    server.broadcastMessage(endOfNamesMsg, channel->getClientFDs(), empty_exclude_fds);
    server.broadcastMessage(modeMsg, channel->getClientFDs(), empty_exclude_fds);

    std::cout << client.getNickname() << " joined channel " << channelName << std::endl;
}
*/


/*
void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("JOIN"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);
    bool isNewChannel = false;

    if (!channel) {
        channel = new Channel(channelName, server);
        server.addChannel(channel);
        isNewChannel = true;
    }

    // Añadir el cliente al canal
    channel->manageUser(&client, PARTICIPANT, true);

    // 1. Enviar confirmación de JOIN
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN :" + channelName + "\r\n";

	std::set<int> exclude_fds;  // Conjunto vacío si no quieres excluir a nadie
	server.broadcastMessage(joinMsg, channel->getClientFDs(), exclude_fds);

    // 2. Enviar el comando TOPIC explícitamente
    std::string topicCmd = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " TOPIC " + channelName + "\r\n";
    server.sendResponse(client.getSocketFD(), topicCmd);

    // Luego enviar la respuesta del tópico
    std::string topicMessage = ":" + server.getServerName() + " 332 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
    server.sendResponse(client.getSocketFD(), topicMessage);

    // 3. Enviar la lista de usuarios (NAMES)
    std::string clientList;
    const std::vector<Client*>& participants = channel->getParticipants();
	for (size_t i = 0; i < participants.size(); ++i) {
		clientList += participants[i]->getNickname();  // Añadir el nick sin el @ si no hay operadores
		if (i < participants.size() - 1) {
			clientList += " ";
		}
	}
	    // Enviar el listado de usuarios como un mensaje de texto normal
    std::string userListMessage = "Users in " + channelName + ": " + clientList + "\r\n";
    server.broadcastMessage(userListMessage, channel->getClientFDs(), exclude_fds); // Enviar a todos los usuarios en el canal



    std::string namesMsg = ":" + server.getServerName() + " 353 " + client.getNickname() + " = " + channelName + " :" + clientList + "\r\n";
    server.sendResponse(client.getSocketFD(), namesMsg);

    std::string endNamesMsg = ":" + server.getServerName() + " 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list\r\n";
    server.sendResponse(client.getSocketFD(), endNamesMsg);


// Crear un conjunto para excluir al nuevo cliente
exclude_fds.insert(client.getSocketFD());  // Agregar el socket del cliente que se acaba de unir

// Enviar el mensaje a todos los demás clientes en el canal, excluyendo al nuevo cliente
server.broadcastMessage(userListMessage, channel->getClientFDs(), exclude_fds);



    // 4. Enviar el modo del canal (normalmente +nt)
    if (!channel->hasSentModeToClient(client)) {  // Verifica si ya se envió el modo
    std::string modeMessage = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt\r\n";
    server.sendResponse(client.getSocketFD(), modeMessage);
    channel->setModeSentToClient(client);  // Marca que ya se envió el modo
	}



    // 5. Enviar la lista de usuarios detallada (WHO)
    // Código para WHO...
}
*/

/*
void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("JOIN"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);
    bool isNewChannel = false;

    if (!channel) {
        channel = new Channel(channelName, server);
        server.addChannel(channel);
        isNewChannel = true;
    }

    // Añadir el cliente al canal
    channel->manageUser(&client, PARTICIPANT, true);

    // 1. Enviar confirmación de JOIN
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN :" + channelName + "\r\n";
    std::set<int> exclude_fds;
    server.broadcastMessage(joinMsg, channel->getClientFDs(), exclude_fds);

    // 2. Enviar el modo del canal
    std::string modeMessage = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt\r\n";
    server.sendResponse(client.getSocketFD(), modeMessage);

    // 3. Enviar el tópico
    std::string topicMessage = ":" + server.getServerName() + " 332 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
    server.sendResponse(client.getSocketFD(), topicMessage);

    // 4. Construir y enviar la lista de usuarios como un mensaje de canal
    std::string clientList;
    const std::vector<Client*>& participants = channel->getParticipants();
    for (size_t i = 0; i < participants.size(); ++i) {
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    }

    // Enviar el listado de usuarios como un mensaje de canal
	std::string namesMsg = ":" + server.getServerName() + " 353 " + client.getNickname() + " = " + channelName + " :" + clientList + "\r\n";
	server.sendResponse(client.getSocketFD(), namesMsg);  // Enviar solo al nuevo cliente

    // Enviar fin de la lista de nombres
	std::string endNamesMsg = ":" + server.getServerName() + " 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list\r\n";
	server.sendResponse(client.getSocketFD(), endNamesMsg);  // Solo al nuevo cliente

}
*/


void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("JOIN"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);

    if (!channel) 
    {
        // Crear un nuevo canal si no existe
        channel = new Channel(channelName, server, client);
        server.addChannel(channel);
    }
    // else if (invite only)
    // {

    // }
    else
    {
        channel->manageUser(&client, PARTICIPANT, true);
    }

    // Añadir el cliente al canal si hay que añadirlo

    // 1. Enviar confirmación de JOIN al cliente que se une
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN :" + channelName + "\r\n";
    
    // Enviar el mensaje de unión al cliente que se acaba de unir (aseguramos que el primero lo vea)
    server.sendResponse(client.getSocketFD(), joinMsg);

    // 2. Enviar el mismo mensaje al resto de usuarios en el canal
    std::set<int> exclude_fds;
    exclude_fds.insert(client.getSocketFD());  // Excluir al cliente que se acaba de unir de recibir el mensaje de su propio JOIN desde otros
    server.broadcastMessage(joinMsg, channel->getClientFDs(), exclude_fds);  // Notificar al resto de usuarios del canal

    // 3. Enviar el modo del canal
    std::string modeMessage = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt\r\n";
    server.sendResponse(client.getSocketFD(), modeMessage);

    // 4. Enviar el tópico
    std::string topicMessage = ":" + server.getServerName() + " 332 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
    server.sendResponse(client.getSocketFD(), topicMessage);

    // 5. Construir y enviar la lista de usuarios como un mensaje de canal (NAMES)
    std::string clientList;
    const std::vector<Client*>& participants = channel->getUsersWithRole("INCHANNEL");
    for (size_t i = 0; i < participants.size(); ++i) {
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    }

    // Enviar la lista de usuarios (NAMES) solo al cliente que acaba de unirse
    std::string namesMsg = ":" + server.getServerName() + " 353 " + client.getNickname() + " = " + channelName + " :" + clientList + "\r\n";
    server.sendResponse(client.getSocketFD(), namesMsg);

    // Enviar fin de la lista de nombres
    std::string endNamesMsg = ":" + server.getServerName() + " 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list\r\n";
    server.sendResponse(client.getSocketFD(), endNamesMsg);
}

