#include "modeCommand.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

void handleModeCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("MODE"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);
    if (tokens.size() == 2 && channel)
    {
        std::string flags;
        flags = channel->getAllModes();
        server.sendResponse(client.getSocketFD(), \
            RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), channel->getName(), flags));
        return;
    }
    else if (tokens.size() == 2 && channel->isUserInChannel(tokens[1]))
    {
        /* code */
    }
    
    if (!channel) {
        server.sendResponse(client.getSocketFD(), ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    
    std::string mode = tokens[2];
    if(mode.length() != 2)
    {}
    // Solo enviar el modo si no se ha enviado aún o si el cliente lo solicita explícitamente
    if (!channel->hasSentModeToClient(client)) {
        std::string modeMessage = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt\r\n";
        server.sendResponse(client.getSocketFD(), modeMessage);
        channel->setModeSentToClient(client);  // Marca que ya se ha enviado el modo
    }
}

