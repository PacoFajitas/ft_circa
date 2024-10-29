/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   whoCommand.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:52:48 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/29 16:52:48 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

void handleWhoCommand(Client& client, const std::string& channelName, Server& server) 
{
    // Obtener los usuarios en el canal
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        server.sendResponse(client.getSocketFD(), ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    if(!channel->isUserInChannel(client.getNickname()))
    {
        server.sendResponse(client.getSocketFD(), ERR_NOTONCHANNEL(server.getServerName(), channel->getName()));
        return ;
    }
    const std::vector<Client*>& users = channel->getUsersWithRole("INCHANNEL");
    for (size_t i = 0; i < users.size(); ++i) {
        if (channel->isUserRole(*users[i], "OPERATOR"))
            server.sendResponse(client.getSocketFD(), RPL_WHOREPLY(server.getServerName(), client.getNickname(), 
                channel->getName(), users[i]->getUsername(), users[i]->getHostname(), users[i]->getNickname(), "@", users[i]->getRealname()));
        else
            server.sendResponse(client.getSocketFD(), RPL_WHOREPLY(server.getServerName(), client.getNickname(), 
                channel->getName(), users[i]->getUsername(), users[i]->getHostname(), users[i]->getNickname(), "", users[i]->getRealname()));
    }
    server.sendResponse(client.getSocketFD(), RPL_ENDOFWHO(server.getServerName(), client.getNickname(), channel->getName()));
}

