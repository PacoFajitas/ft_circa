#include "utils.hpp"
#include "responses.hpp"
#include <iostream>
#include "privmsgCommand.hpp"
#include "server.hpp"
#include "responses.hpp"
#include "client.hpp"
#include "channel.hpp"

void handlePrivmsgCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
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
    std::cout << "holiiiii" << std::endl;
    std::string formattedMessage = ":" + client.getNickname() + " PRIVMSG " + recipientNick + " :" + message;
    if (recipientClient)
        server.sendResponse(recipientClient->getSocketFD(), formattedMessage);
    else
        recipientChannel->sendMessage(formattedMessage, client.getSocketFD()); 
}


/*
void handlePrivmsgCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 3) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "PRIVMSG"));
        return;
    }

    std::string recipientNick = tokens[1];
    std::string message = joinTokens(tokens, 2);


//////////////////////AQUI empieza el cambio

	if (message.find("DCC SEND") != std::string::npos) {
		std::cout << "-----------------------------" << std::endl;
		std::cout << "DCC SEND command detected: " << message << std::endl;
		std::cout << "-----------------------------" << std::endl;
		std::vector<std::string> dccTokens = splitString(message, " ");
        if (dccTokens.size() < 5) {
			server.sendResponse(client.getSocketFD(), "Error: Invalid DCC SEND command format.");
			return;
		}

		std::string fileName = dccTokens[2];
        std::string ip = dccTokens[3];
        std::string port = dccTokens[4];
		std::streamsize fileSize;
		std::stringstream(dccTokens[5]) >> fileSize;


		Client* recipientClient = server.getClientByNick(recipientNick);
        if (!recipientClient) {
            server.sendResponse(client.getSocketFD(), ERR_NOSUCHNICK(server.getServerName(), recipientNick));
            return;
        }
		// Iniciar el proceso de transferencia o enviar la respuesta al cliente
        server.addFileTransfer(&client, recipientClient, fileName, fileSize);

        // Confirmación
        server.sendResponse(client.getSocketFD(), "150 " + recipientNick + " " + fileName + " :DCC SEND initiated.");
        server.sendResponse(recipientClient->getSocketFD(), "DCC SEND request for file " + fileName + " from " + client.getNickname());
	}
	else {
		Client* recipientClient = server.getClientByNick(recipientNick);
        // Si no es un DCC SEND, procesar el mensaje normal
        std::string formattedMessage = ":" + client.getNickname() + " PRIVMSG " + recipientNick + " :" + message;
		server.sendResponse(recipientClient->getSocketFD(), formattedMessage);
    }
}


void handleDCCSend(Client& sender, Client* recipient, const std::string& message, Server& server) {
    std::cout << "Processing DCC SEND..." << std::endl;
    // Aquí podríamos extraer la información del archivo (nombre, tamaño, etc.) del mensaje DCC

    // Extraemos el nombre del archivo y otros parámetros del mensaje.
    std::vector<std::string> dccTokens = splitString(message, " ");
    if (dccTokens.size() < 5) {
        server.sendResponse(sender.getSocketFD(), "Error: DCC SEND command malformed.");
        return;
    }

    std::string fileName = dccTokens[2];
	std::stringstream ss(dccTokens[4]);
	std::streamsize fileSize;
	ss >> fileSize;


    // Guardamos la transferencia de archivos en el servidor
    server.addFileTransfer(&sender, recipient, fileName, fileSize);

    // Notificamos al destinatario sobre la transferencia
    std::string dccNotification = sender.getNickname() + " wants to send you the file: " + fileName;
    server.sendResponse(recipient->getSocketFD(), dccNotification);
}
*/
