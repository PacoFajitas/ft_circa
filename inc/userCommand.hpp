#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include <string>
#include <vector>
#include "client.hpp"
#include "server.hpp"

bool    handleUserCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif // USERCOMMAND_HPP
