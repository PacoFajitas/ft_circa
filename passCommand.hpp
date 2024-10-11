#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "client.hpp"
#include "server.hpp"
#include <vector>
#include <string>

bool handlePassCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif // PASSCOMMAND_HPP

