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
    if(tokens.size() - 3 > 2 && mode.length() < 2)
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
    else
    {
        bool active;
        ulong numPar = 0; //Para contar cuantos modos necesitan parametros
        if (mode.at(0) == '-')
            active = false;
        else if (mode.at(0) == '+')
            active = true;
        else
            server.sendResponse(client.getSocketFD(), "ERR_UNKNOWNERROR (400) : Sign needed to set modes");
        bool checkargs = false;
        for (ulong i = 1; i < mode.length(); i++)
        {
            if (mode[i] == 'k' || mode[i] == 'o') //modificar isalnum por la funcion de valildchars
            {
                numPar++;
                checkargs = true;
            }
            else if (mode[i] == 'l' && (tokens.size() - 3 - (numPar + 1) >= 0) && isStringNum(tokens[3 + numPar + 1])) //:servidor 467 <tu_nick> #canal :Channel limit is not a valid number
            {
                checkargs = true;
                numPar++;
            }
            else if (mode[i] == 'l' && (tokens.size() - 3 - (numPar + 1) >= 0) && !isStringNum(tokens[3 + numPar + 1])) 
            {
                server.sendResponse(client.getSocketFD(), "ERR_UNKNOWNERROR (400) : Please put numbers as arguments for key +l");
                return ;
            }
            else if( mode[i] != 't' && mode [i] != 'i')
            {
                server.sendResponse(client.getSocketFD(), ERR_UNKNOWNMODE(server.getServerName(), client.getNickname(), mode[i]));
                return ;
            }
        }
        // MODE #channel +iiiolkl
        // MODE #channel +iiiolk argO argL argK argL etc. -->numPar=4+3 tokensize >= 7
        std::cout << "MODE LENGTH    " << mode.length() << std::endl;
        if (tokens.size() < numPar + 3)
        {
            server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
        }
        else
        {
            std::cout << "holi(?)" << std::endl;
            server.sendResponse(client.getSocketFD(), "Holi");
            for (size_t i = 1; i < mode.length(); i++)
            {
                if (mode.at(i) == 't' || mode.at(i) == 'i')
                {
                    channel->setMode(mode.at(i), active);
                }
                else if(mode.at(i) == 'o' || mode.at(i) == 'l' || mode.at(i) == 'k')
                {
                    channel->setMode(mode.at(i), active, tokens[i + 2]);
                    numPar--;
                }
            }
        }
    }
    
    // Solo enviar el modo si no se ha enviado aún o si el cliente lo solicita explícitamente
    if (!channel->hasSentModeToClient(client)) 
    {
        // std::string modeMessage = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt\r\n";
        server.sendResponse(client.getSocketFD(), RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), channel->getName(), channel->getAllModes()));
        channel->setModeSentToClient(client);  // Marca que ya se ha enviado el modo
    }
}

