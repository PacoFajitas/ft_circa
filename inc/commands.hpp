// commands.hpp
#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <vector>
#include <string>
#include "client.hpp"
#include "server.hpp"

void    logCommandProcessing(const std::string& command);
bool	processCommand(const std::string& command, Client& client, Server& server);

#endif // COMMANDS_HPP

