#include "modeCommand.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

//Estaria bien moverlo a su sitio y tal cuanndo este guay la funcion :D
bool isStringNum(std::string str)
{
    for (ulong i = 0; i < str.length(); i++)
    {
        if (!isdigit(str[i]))
            return false;
    }
    return true;
}

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
        ulong numPar = 0; //Para contar cuantos modos necesitan parametros
        if (mode.at(0) == '-')
            active = false;
        else if (mode.at(0) == '+')
            active = true;
        else
        {
            //si no tiene signo y no existe tira error
            // si no tiene signo y existe devuelve la lista de modos activos
        }
        for (ulong i = 1; i < mode.length(); i++)
        {
            if (mode[i] == 'k' || mode[i] == 'o') //modificar isalnum por la funcion de valildchars
            {
                numPar++;
            }
            else if (mode[i] == 'l' && (tokens.size() - 3 - (numPar + 1) >= 0) && isStringNum(tokens[3 + numPar + 1])) //:servidor 467 <tu_nick> #canal :Channel limit is not a valid number
                numPar++;
            else if (mode[i] == 'l' && (tokens.size() - 3 - (numPar + 1) >= 0) && !isStringNum(tokens[3 + numPar + 1])) 
                server.sendResponse(client.getSocketFD(), "aaaaaaa");
//                 ERR_UNKNOWNERROR (400) 
//   "<client> <command>{ <subcommand>} :<info>"
// Indicates that the given command/subcommand could not be processed. <subcommand> may repeat for more specific subcommands.


                //:servidor 467 <tu_nick> #canal :Channel limit is not a valid number
        }
        if (numPar <= tokens.size() - 3)
        {
            // ejecuta
        }        
        // server.sendResponse(client.getSocketFD(), ERR_UNKNOWNMODE(server.getServerName(), client.getNickname(), mode[i]));
        }
    
    
    // Solo enviar el modo si no se ha enviado aún o si el cliente lo solicita explícitamente
    if (!channel->hasSentModeToClient(client)) 
    {
        // std::string modeMessage = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt\r\n";
        server.sendResponse(client.getSocketFD(), RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), channel->getName(), channel->getAllModes()));
        channel->setModeSentToClient(client);  // Marca que ya se ha enviado el modo
    }
}

