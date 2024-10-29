/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsgCommand.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:48:47 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/29 16:49:39 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"
#include "utils.hpp"

void handlePrivmsgCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
    if (tokens.size() < 3) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "PRIVMSG"));
        return;
    }

    std::string recipientNick = tokens[1];
    std::string message = joinTokens(tokens, 2);

    Client* recipientClient = server.getClientByNick(recipientNick);
    Channel* recipientChannel = server.getChannel(recipientNick);
    if (!recipientClient && !recipientChannel) {
        server.sendResponse(client.getSocketFD(), ERR_NOSUCHNICK(server.getServerName(), recipientNick));
        return;
    }
    if (recipientClient)
        server.sendResponse(recipientClient->getSocketFD(), RPL_PRIVMSG(client.getNickname(), recipientClient->getNickname(), message));
    else
        recipientChannel->sendMessage(RPL_PRIVMSG(client.getNickname(), recipientNick, message), client.getSocketFD()); 
}
