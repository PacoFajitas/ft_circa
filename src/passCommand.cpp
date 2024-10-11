#include "passCommand.hpp"
#include "responses.hpp"

bool handlePassCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("PASS"));
        return false;
    }

    if (client.isAuthenticated()) {
        server.sendResponse(client.getSocketFD(), ERR_ALREADYREGISTRED());
        return false;
    }

    std::string password = tokens[1];
    if (password != server.getPassword()) {
        server.sendResponse(client.getSocketFD(), ERR_PASSWDMISMATCH());
        return false;
    }

    client.setAuthenticated(true);
    return true;
}

