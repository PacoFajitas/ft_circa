// commands.cpp
#include "commands.hpp"
#include "utils.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "passCommand.hpp"
#include <iostream>
#include "nickCommand.hpp"
#include "userCommand.hpp"
#include "channel.hpp"
#include "joinCommand.hpp"
#include "pingCommand.hpp"
#include "pongCommand.hpp"
#include "whoCommand.hpp"
#include "modeCommand.hpp"
#include "partCommand.hpp"
bool	requireRegistration(Client& client, Server& server) {
	if (!client.isFullyRegistered()) {
		server.sendResponse(client.getSocketFD(), ERR_NOTREGISTERED()); // Cliente no registrado
		return (false);
	}
	return (true);
}


bool	processCommand(const std::string& command, Client& client, Server& server) {
	std::vector<std::string> tokens = splitString(command, " ");
    if (tokens.empty())
		return (false);

    std::string commandName = tokens[0];

    if (commandName == "NICK") {
		std::cout << "-----------------------------" << std::endl;
        std::cout << "Processing NICK command: " << command << std::endl;
		std::cout << "-----------------------------" << std::endl;
        handleNickCommand(client, tokens, server);
    } else if (commandName == "PASS") {
		std::cout << "-----------------------------" << std::endl;
        std::cout << "Processing PASS command: " << command << std::endl;
		std::cout << "-----------------------------" << std::endl;
        handlePassCommand(client, tokens, server);
    } else if (commandName == "USER") {
		std::cout << "-----------------------------" << std::endl;
        std::cout << "Processing USER command: " << command << std::endl;
		std::cout << "-----------------------------" << std::endl;
        handleUserCommand(client, tokens, server);
	}
	else {
		if ((commandName == "CAP" || commandName == "PING") && !requireRegistration(client, server)) {
			std::cout << "Ignoring command: " << command << std::endl;
			return (true);  // Ignorar el comando CAP
			}
		if (!requireRegistration(client, server))
			return (false);

		if (commandName == "JOIN") {
			std::cout << "-----------------------------" << std::endl;
			std::cout << "Processing JOIN command: " << command << std::endl;
			std::cout << "-----------------------------" << std::endl;
			handleJoinCommand(client, tokens, server);
			}
		else if (commandName == "PING") {
			handlePingCommand(client, tokens, server);
			}
		else if (commandName == "PONG" && client.isRegistered()) {
			handlePongCommand(tokens);
			}
		else if (commandName == "WHO") {
			if (tokens.size() > 1) {
				std::cout << "-----------------------------" << std::endl;
				std::cout << "Processing WHO command: " << command << std::endl;
				std::cout << "-----------------------------" << std::endl;
				handleWhoCommand(client, tokens[1], server);
				}
		}
		else if (commandName == "MODE") {
			std::cout << "-----------------------------" << std::endl;
			std::cout << "Processing MODE command: " << command << std::endl;
			std::cout << "-----------------------------" << std::endl;
			handleModeCommand(client, tokens, server);
			}
		else if(commandName == "PART"){
			std::cout << "-----------------------------" << std::endl;
			std::cout << "Processing PART command: " << command << std::endl;
			std::cout << "-----------------------------" << std::endl;
			handlePartCommand(client, tokens, server);
			}
		}
	return (true);
}




