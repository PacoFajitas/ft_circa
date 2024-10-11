#include "whoCommand.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

void handleWhoCommand(Client& client, const std::string& channelName, Server& server) {
    // Obtener los usuarios en el canal
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        // Si el canal no existe, podemos retornar un error o simplemente no hacer nada
        return;
    }

    const std::vector<Client*>& users = channel->getParticipants();
    for (size_t i = 0; i < users.size(); ++i) {
        // Enviar respuesta WHO (352) para cada usuario en el canal
        std::string whoReplyMsg = ":" + server.getServerName() + " 352 " + client.getNickname() +
                                  " " + channelName + " " + users[i]->getUsername() + " " + 
                                  users[i]->getHostname() + " " + server.getServerName() + " " +
                                  users[i]->getNickname() + " H :0 " + users[i]->getRealname();
        server.sendResponse(client.getSocketFD(), whoReplyMsg);
    }

    // Fin del WHO (315)
    std::string endOfWhoMsg = ":" + server.getServerName() + " 315 " + client.getNickname() +
                              " " + channelName + " :End of /WHO list";
    server.sendResponse(client.getSocketFD(), endOfWhoMsg);
}

