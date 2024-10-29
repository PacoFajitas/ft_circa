/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   modeCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:36:41 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/29 16:40:17 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"
#include "utils.hpp"

static int countModeArgs(std::vector<std::string> arg)
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

static std::vector<std::string> getArgs(std::vector<std::string> arg)
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

static int countModeFlags(std::string arg, bool active)
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

void handleModeCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
        return;
    }
    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);
    std::string err;
    if (!channel)
        err = ERR_NOSUCHCHANNEL(channelName);
    else if (tokens.size() == 2 && channel)
        err = RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), 
            channel->getName(), channel->getAllModes());
    else if (!channel->isUserRole(client, "OPERATOR"))
        err = ERR_CHANOPRIVSNEEDED(server.getServerName(), channel->getName());
    if (!err.empty())
    {
        server.sendResponse(client.getSocketFD(), err);
        return ;
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
    numPar = 0;
    std::string clientList;
    for (size_t i = 1; i < mode.length(); i++)
    {
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
            }
            else if (!active)
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



