#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include <iostream>

#ifndef WHOCOMMAND_HPP
#define WHOCOMMAND_HPP

void handleWhoCommand(Client& client, const std::string& channelName, Server& server);

#endif

