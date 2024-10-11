#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "responses.hpp"
#include "utils.hpp"
#include "pingCommand.hpp"
#include <iostream>

void	handlePingCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        return;  // Si el PING no tiene un parámetro, lo ignoramos
    }
	std::cout << "Recibido PING del cliente " << client.getNickname() << std::endl;  // Depuración
    // Crear el mensaje de respuesta PONG
    std::string response = ":" + server.getServerName() + " PONG :" + tokens[1];
    server.sendResponse(client.getSocketFD(), response);  // Enviar el PONG de vuelta al cliente
}

