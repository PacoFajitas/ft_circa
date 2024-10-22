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
        server.sendResponse(client.getSocketFD(), \
            RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), channel->getName(), flags));
        return;
    }

    std::string mode = tokens[2];
    bool active;
    if (mode.at(0) == '-')
        active = false;
    else if (mode.at(0) == '+')
        active = true;
    else
    {
        server.sendResponse(client.getSocketFD(), ERR_UNKNOWNMODE(server.getServerName(), client.getNickname(), " no flag "));
        return ;
    }
    // mode -lllllllllllllllllllllllllllllllllllllllllllllllo meri
    if(countModeArgs(tokens) < countModeFlags(mode, active))
    {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE1"));
        return ;
    }
    std::vector<std::string> args = getArgs(tokens);
    bool checkargs = false;
    ulong numPar = 0; //Para contar cuantos modos necesitan parametros
    for (ulong i = 1; i < mode.length(); i++)
    {
        // server.sendResponse(client.getSocketFD(), "Holi");
        // std::cout << (tokens[3 + numPar]) << std::endl;
        if (mode[i] == 'k' || mode[i] == 'o') //modificar isalnum por la funcion de valildchars
        {
            numPar++;
            checkargs = true;
        }
        else if (active && mode[i] == 'l' && isStringNum(args[numPar])) //:servidor 467 <tu_nick> #canal :Channel limit is not a valid number
        {
            checkargs = true;
            numPar++;
        }
        else if (active && mode[i] == 'l' && !isStringNum(args[numPar])) 
        {
            server.sendResponse(client.getSocketFD(), std::string("ERR_UNKNOWNERROR (400) : Please put numbers as arguments for key +l jiji" + tokens[3 + numPar]));
            return ;
        }
        else if( mode[i] != 't' && mode [i] != 'i'&& mode [i] != 'l' )
        {
            server.sendResponse(client.getSocketFD(), ERR_UNKNOWNMODE(server.getServerName(), client.getNickname(), mode[i]));
            return ;
        }
    }
    // MODE #channel +iiiolkl
    // MODE #channel +iiiolk argO argL argK argL etc. -->numPar=4+3 tokensize >= 7
    // std::cout << "MODE LENGTH    " << mode.length() << std::endl;
    numPar = 0;
    std::string clientList;
    for (size_t i = 1; i < mode.length(); i++)
    {
        // if (active == true)
        //     server.sendResponse(client.getSocketFD(), "Verdad");
        // else
        //     server.sendResponse(client.getSocketFD(), "Feka como Arzel");
        if (mode.at(i) == 't' || mode.at(i) == 'i')
        {
            channel->setMode(mode.at(i), active);
        }
        else if(mode.at(i) == 'o' || mode.at(i) == 'l' || mode.at(i) == 'k')
        {
            if (mode.at(i) == 'o')
            {
                if(channel->isUserInChannel(args[numPar]))
                {
                    channel->setMode(mode.at(i), active, args[numPar]);
                    if (active)
                        channel->sendMessage(RPL_OPERATOR(client.getNickname(), client.getUsername(), client.getHostname(),
                            channel->getName(), "+", args[numPar]), -1);
                    else
                        channel->sendMessage(RPL_OPERATOR(client.getNickname(), client.getUsername(), client.getHostname(),
                            channel->getName(), "-", args[numPar]), -1);
                    clientList = channel->clientOpList();
                }
                else
                    server.sendResponse(client.getSocketFD(), ERR_USERNOTINCHANNEL(server.getServerName(), args[numPar], channel->getName()));
                numPar++;
            }else if (!active)
                channel->setMode(mode.at(i), active);
            else
            {
                channel->setMode(mode.at(i), active, args[numPar]);
                numPar++;
            }
        }
    }
    server.sendResponse(client.getSocketFD(), RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), channel->getName(), channel->getAllModes()));
    if (!clientList.empty())
    {
        channel->sendMessage(RPL_NAMREPLY(server.getServerName(), client.getNickname(), channel->getName(), clientList), -1);
        channel->sendMessage(RPL_ENDOFNAMES(client.getNickname(),channel->getName()), -1);
    }
}

int countModeFlags(std::string arg, bool active)
{
    int ret = 0;
    for (size_t i = 0; i < arg.length(); i++)
    {
        if (active && arg.at(i) != '+' && arg.at(i) != '-' && arg.at(i) != 'i' && arg.at(i) != 't')
            ret++;
        else if(!active && arg.at(i) == 'o')
            ret++;
        
    }
	return ret;
}

int countModeArgs(std::vector<std::string> arg)
{
    int ret = 0;
    bool startCount = false;
    std::vector<std::string>::iterator it;

    it = arg.begin();
    while (it != arg.end())
    {
        if (startCount)
            ret++;
        if (it->at(0) == '-' || it->at(0) == '+')
            startCount = true;
        ++it; 
    }    
	return ret;
}
std::vector<std::string> getArgs(std::vector<std::string> arg)
{
    std::vector<std::string> ret;
    bool startCount = false;
    std::vector<std::string>::iterator it;

    it = arg.begin();
    while (it != arg.end())
    {
        if (startCount)
            ret.push_back(*it);
        if (it->at(0) == '-' || it->at(0) == '+')
            startCount = true;
        ++it; 
    }
    return ret;
}
