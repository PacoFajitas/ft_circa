/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processCommand.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:51:11 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/31 17:48:52 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"
#include "utils.hpp"

static void printCommands(std::string command)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << "Processing " << command << " command" << std::endl;
	std::cout << "-----------------------------" << std::endl;
}

static bool	requireRegistration(Client& client, Server& server) {
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
		printCommands("NICK");
        handleNickCommand(client, tokens, server);
    } else if (commandName == "PASS") {
		printCommands("PASS");
        handlePassCommand(client, tokens, server);
    } else if (commandName == "USER") {
		printCommands("USER");
        handleUserCommand(client, tokens, server);
	}
	else {
		if ((commandName == "CAP" || commandName == "PING") && !requireRegistration(client, server)) {
			std::cout << "Ignoring command: " << command << std::endl;
			return (true);
			}
		if (!requireRegistration(client, server))
			return (false);

		if (commandName == "JOIN") {
			printCommands("JOIN");
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
				printCommands("WHO");
				handleWhoCommand(client, tokens[1], server);
				}
		}
		else if (commandName == "MODE") {
			printCommands("MODE");
			handleModeCommand(client, tokens, server);
		}
		else if(commandName == "PART"){
			printCommands("PART");
			handlePartCommand(client, tokens, server);
		}
		else if(commandName == "PRIVMSG"){
			printCommands("PRIVMSG");
			handlePrivmsgCommand(client, tokens, server);
		}
		else if(commandName == "INVITE"){
			printCommands("INVITE");
			handleInviteCommand(client, tokens, server);
		}
		else if(commandName == "KICK"){
			printCommands("KICK");
			handleKickCommand(client, tokens, server);
		}
		else if(commandName == "TOPIC"){
			printCommands("TOPIC");
			handleTopicCommand(client, tokens, server);
		}
		else
		{
			std::cout << "-----------------------------" << std::endl;
			std::cout << "Unknown command: " << command << std::endl;
			std::cout << "-----------------------------" << std::endl;
			server.sendResponse(client.getSocketFD(), ERR_UNKNOWNCOMMAND(commandName));
		}
	}
	return (true);
}




