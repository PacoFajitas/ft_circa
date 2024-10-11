#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include <iostream>

void	handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server);
void	whoServer(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif
