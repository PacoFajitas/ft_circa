/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pingCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:46:28 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/29 16:46:32 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

void	handlePingCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
    if (tokens.size() < 2) {
        return;  // Si el PING no tiene un parámetro, lo ignoramos
    }
	std::cout << "Recibido PING del cliente " << client.getNickname() << std::endl;  // Depuración
    // Crear el mensaje de respuesta PONG
    std::string response = ":" + server.getServerName() + " PONG :" + tokens[1];
    server.sendResponse(client.getSocketFD(), response);  // Enviar el PONG de vuelta al cliente
}

