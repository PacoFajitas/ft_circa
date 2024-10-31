/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joinCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:33:16 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/30 18:18:54 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"
#include "utils.hpp"

void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) 
{
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);
    std::string err;
    if (!channel)
    {
        if (!server.isValidNickChan(channelName, true))
        {
            server.sendResponse(client.getSocketFD(), ERR_NOSUCHCHANNEL(channelName));
            return ;
        }
        channel = new Channel(channelName, server, client, server.getBot());
        server.addChannel(channel);
    }
    else if (channel->getMode('i') == true && !channel->isUserRole(client, "INVITED"))
        err = ERR_INVITEONLYCHAN(channel->getName());
    else if (channel->getMode('l') && channel->getUsersWithRole("INCHANNEL").size() == (unsigned)channel->getLimitUsers())
        err = ERR_CHANNELISFULL(client.getNickname(), channel->getName());
    else if (channel->getMode('k') && tokens.size() < 3)
        err = ERR_BADCHANNELKEY(client.getNickname(), channel->getName());
    else if (channel->getMode('k') && tokens.size() >= 3 && tokens[2] != channel->getPassword())
        err = ERR_BADCHANNELKEY(client.getNickname(), channel->getName());
    else
        channel->manageUser(&client, PARTICIPANT, true);
    if (!err.empty())
    {
        server.sendResponse(client.getSocketFD(), err);
        return ;
    }
    
    server.sendResponse(client.getSocketFD(), 
        RPL_JOIN(client.getNickname(), client.getUsername(), client.getHostname(), channel->getName()));
   
    channel->sendMessage(RPL_JOIN(client.getNickname(), client.getUsername(), client.getHostname(), 
        channel->getName()), client.getSocketFD());
    if (!channel->getTopic().empty())
        server.sendResponse(client.getSocketFD(), RPL_TOPIC(server.getServerName(), client.getNickname(), channel->getName(), channel->getTopic()));

    std::string clientList = channel->clientOpList();
    server.sendResponse(client.getSocketFD(), RPL_NAMREPLY(server.getServerName(), client.getNickname(), channel->getName(), clientList));
    server.sendResponse(client.getSocketFD(), RPL_ENDOFNAMES(client.getNickname(),channel->getName()));
    std::cout << client.getUsername() << std::endl;
    //Mensaje de bienvenida del bot al canal
    server.sendBotWelcome(&client, channel);
}

