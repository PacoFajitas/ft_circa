#include "modeCommand.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

void handleModeCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);
    
    if (!channel) {
        server.sendResponse(client.getSocketFD(), ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    if (tokens.size() == 2 && channel)
    {
        std::string flags;
        flags = channel->getAllModes();
        // std::cout << "HOLIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII" << channel->getAllModes() << std::endl;
        server.sendResponse(client.getSocketFD(), \
            RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), channel->getName(), flags));
        return;
    }

    std::string mode = tokens[2];
    if(tokens.size() > 2 && mode.length() < 2)
    {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
    }
    else
    {
        bool active;
        if (mode.at(0) == '-')
            active = false;
        else if (mode.at(0) == '+')
            active = true;
        else
        {
            //si no tiene signo y no existe tira error
            // si no tiene signo y existe devuelve la lista de modos activos
        }
        // for (int i = 1; i < mode.length(); i++)
        // {
        //     if (mode[i] == 'i' || mode[i] == 't' || mode[i] == 'k' || mode[i] == 'o' || mode[i] == 'l')
        //         channel->setMode(mode[i], active, msg);
        //     else
        //         server.sendResponse(client.getSocketFD(), ERR_UNKNOWNMODE(server.getServerName(), client.getNickname(), mode[i]));
        // }
    }
    
    // Solo enviar el modo si no se ha enviado aún o si el cliente lo solicita explícitamente
    if (!channel->hasSentModeToClient(client)) {
        std::string modeMessage = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt\r\n";
        server.sendResponse(client.getSocketFD(), modeMessage);
        channel->setModeSentToClient(client);  // Marca que ya se ha enviado el modo
    }
}

