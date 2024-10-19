#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include "utils.hpp"
#include <iostream>

#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

void	handleModeCommand(Client& client, const std::vector<std::string>& tokens, Server& server);
int		countModeFlags(std::string arg);
int		countModeArgs(std::vector<std::string> arg);
std::vector<std::string> getArgs(std::vector<std::string> arg);

#endif

