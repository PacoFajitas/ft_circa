/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   passCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:45:58 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/31 17:49:55 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

bool handlePassCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "PASS"));
        return false;
    }
    if (client.isAuthenticated()) {
        server.sendResponse(client.getSocketFD(), ERR_ALREADYREGISTRED());
        return false;
    }
    std::string password = tokens[1];
    if (password != server.getPassword()) {
        server.sendResponse(client.getSocketFD(), ERR_PASSWDMISMATCH());
        return false;
    }
    client.setAuthenticated(true);
    return true;
}

