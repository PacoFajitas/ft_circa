// nickCommand.hpp
#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include <string>
#include <vector>
#include "client.hpp"
#include "server.hpp"


bool    validateNickname(const std::string& nickname, Client& client, Server& server);

void    changeNickname(Client& client, const std::string& nickname, Server& server);

void    registerClient(Client& client, Server& server);

bool    isNicknameInUse(const std::string& nickname, const Server& server);

// Declaración de la función para manejar el comando NICK
bool	handleNickCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif // NICKCOMMAND_HPP

