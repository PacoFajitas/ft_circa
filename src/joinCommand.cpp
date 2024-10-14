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

void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN"));
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
    else if (channel->getMode('i') == true && !channel->isUserRole(client, "INVITED"))
        server.sendResponse(client.getSocketFD(),ERR_INVITEONLYCHAN(channel->getName()));
    else
        channel->manageUser(&client, PARTICIPANT, true);

    // 1. Enviar confirmación de JOIN al cliente que se une
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN :" + channelName + "\r\n";
    
    // Enviar el mensaje de unión al cliente que se acaba de unir (aseguramos que el primero lo vea)
    server.sendResponse(client.getSocketFD(), joinMsg);

    // 2. Enviar el mismo mensaje al resto de usuarios en el canal
    std::set<int> exclude_fds;
    exclude_fds.insert(client.getSocketFD());  // Excluir al cliente que se acaba de unir de recibir el mensaje de su propio JOIN desde otros
    server.broadcastMessage(joinMsg, channel->getClientFDs(), exclude_fds);  // Notificar al resto de usuarios del canal

    if (!channel->getTopic().empty())
        server.sendResponse(client.getSocketFD(), RPL_TOPIC(server.getServerName(), channel->getName(), channel->getTopic()));

    // 5. Construir y enviar la lista de usuarios como un mensaje de canal (NAMES)
    std::string clientList;
    const std::vector<Client*>& participants = channel->getUsersWithRole("INCHANNEL");
    for (size_t i = 0; i < participants.size(); ++i) {
        if (channel->isUserRole(*participants[i], "OPERATOR"))
            clientList += "@";
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    }

    // Enviar la lista de usuarios (NAMES) solo al cliente que acaba de unirse
    server.sendResponse(client.getSocketFD(), RPL_NAMREPLY(server.getServerName(), client.getNickname(), channel->getName(), clientList));

    // Enviar fin de la lista de nombres
    server.sendResponse(client.getSocketFD(), RPL_ENDOFNAMES(client.getNickname(),channel->getName()));
}

