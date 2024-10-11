// userCommand.cpp
#include "commands.hpp"
#include "responses.hpp"
#include "client.hpp"
#include "server.hpp"


bool handleUserCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 5) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("USER"));
        return false;
    }

    // Configurar los atributos del cliente
    client.setUsername(tokens[1]);
    client.setRealname(tokens[4]);

    // Configurar el hostname, en este caso usando una IP fija como ejemplo
    std::string hostname = "127.0.0.1";
    client.setHostname(hostname);

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
        }
    }

    return true;
}

