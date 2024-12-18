/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   userCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:52:19 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/31 17:45:35 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

bool handleUserCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
    if (tokens.size() < 5) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "USER"));
        return false;
    }
    // Configurar los atributos del cliente
    client.setUsername(tokens[1]);
    client.setRealname(tokens[4]);
    // Verificar si el cliente está completamente registrado
    if (client.isFullyRegistered()) {
		server.sendPing(client);
        client.setRegistered(true);
        // Enviar los mensajes de bienvenida al cliente solo si aún no se han enviado
        if (!client.hasReceivedWelcome()) {
            client.setReceivedWelcome(true);
            server.sendResponse(client.getSocketFD(), RPL_WELCOME(client.getNickname(), server.getServerName(), client.getHostname()));
            server.sendResponse(client.getSocketFD(), RPL_YOURHOST(server.getServerName(), client.getNickname()));
            server.sendResponse(client.getSocketFD(), RPL_CREATED(server.getServerName(), server.getTime()));
            server.sendResponse(client.getSocketFD(), RPL_MYINFO(server.getServerName(), client.getNickname()));
            server.sendBotWelcome(client.getSocketFD(), server, client.getNickname());
        }
    }
    return true;
}
