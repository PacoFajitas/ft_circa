#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include <iostream>

void	handlePingCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif

