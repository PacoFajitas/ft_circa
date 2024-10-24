#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "responses.hpp"
#include "bot.hpp"
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
        channel = new Channel(channelName, server, client, server.getBot());
        server.addChannel(channel);
    }
    else if (channel->getMode('i') == true && !channel->isUserRole(client, "INVITED"))
    {
        server.sendResponse(client.getSocketFD(),ERR_INVITEONLYCHAN(channel->getName()));
        return ;
    }
    else
        channel->manageUser(&client, PARTICIPANT, true);

    server.sendResponse(client.getSocketFD(), 
        RPL_JOIN(client.getNickname(), client.getUsername(), client.getHostname(), channel->getName()));

    channel->sendMessage(RPL_JOIN(client.getNickname(), client.getUsername(), client.getHostname(), 
        channel->getName()), client.getSocketFD());
    
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

    //Mensaje de bienvenida del bot al canal
    server.sendResponse(client.getSocketFD(),RPL_PRIVMSG(channel->getBot()->getNickname(), channel->getName(), MESSAGE1));
}

