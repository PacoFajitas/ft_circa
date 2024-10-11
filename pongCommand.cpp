#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "responses.hpp"
#include "utils.hpp"
#include "pongCommand.hpp"
#include <iostream>


void	handlePongCommand(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return;  // Si el PONG no tiene parámetros, lo ignoramos
    }
}

