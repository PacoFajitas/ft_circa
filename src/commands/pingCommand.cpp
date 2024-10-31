/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pingCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:46:28 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/31 17:49:49 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

void	handlePingCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
    if (tokens.size() < 2) {
        return;
    }
	std::cout << "Recibido PING del cliente " << client.getNickname() << std::endl;
    std::string response = ":" + server.getServerName() + " PONG :" + tokens[1];
    server.sendResponse(client.getSocketFD(), response);
}

