/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:28:49 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/29 16:52:38 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

// LIBRARIES
#include <iostream>
#include <string>
#include <vector>

// INCLUDES
#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"
#include "responses.hpp"


// processCommand.cpp
bool	processCommand(const std::string& command, Client& client, Server& server);

// inviteCommand.cpp
void handleInviteCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// joinCommand.cpp
void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// kickCommand.cpp
void handleKickCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// modeCommand.cpp
void handleModeCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// nickCommand.cpp
bool	handleNickCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// partCommand.cpp
void handlePartCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// passCommands.cpp
bool handlePassCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// pingCommand.cpp
void	handlePingCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// pongCommand.cpp
void	handlePongCommand(const std::vector<std::string>& tokens);

// privmsgCommand.cpp
void handlePrivmsgCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// topicCommand.cpp
void handleTopicCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// userCommand.cpp
bool handleUserCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

// whoCommand.cpp
void handleWhoCommand(Client& client, const std::string& channelName, Server& server);

#endif

