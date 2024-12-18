// client.cpp
#include "client.hpp"
#include "utils.hpp" // Asegúrate de incluir para usar convertToCRLF y splitString
#include "commands.hpp"
#include <unistd.h> // Para close()
#include <iostream> // Para std::cout
#include <sys/socket.h> // Para send(), recv()
#include <cstring> // Para manejo de cadenas


Client::Client(int fd) : socket_fd(fd), authenticated(false), registered(false), received_welcome(false), received_botjoin(false), buffer(""){}


int Client::getSocketFD() const {
    return socket_fd;
}

const std::string& Client::getNickname() const {
    return nickname;
}

const std::string& Client::getUsername() const {
    return username;
}

const std::string& Client::getRealname() const {
    return realname;
}

const std::string& Client::getHostname() const { return hostname; }

bool Client::getIsConnected() const
{
    return isConnected;
}

bool Client::isAuthenticated() const {
    return authenticated;
}

bool Client::getHasPassword() const {
    return authenticated;
}

bool Client::isRegistered() const {
    return registered;
}

bool Client::isFullyRegistered() const {
	return (isAuthenticated() && !getNickname().empty() && !getUsername().empty());
}

bool Client::canBeRegistered() const {
	return (!nickname.empty() && !username.empty() && isAuthenticated());
}

void Client::setNickname(const std::string& nick) {
    nickname = nick;
}

void    Client::setConnected(const bool status)
{
    isConnected = status;
}

void Client::setUsername(const std::string& user) {
    username = user;
}

void Client::setRealname(const std::string& real) {
    realname = real;
}

void Client::setHostname(const std::string& hostname) {
	this->hostname = hostname;
}

void Client::setAuthenticated(bool state) {
    authenticated = state;
}

void Client::setRegistered(bool state) {
    registered = state;
}

void	Client::setReceivedWelcome(bool status) {
	received_welcome = status;
}

void	Client::setReceivedBotJoin(bool status) {
	received_botjoin = status;
}

bool	Client::hasReceivedWelcome() const {
	return (received_welcome);
}

bool	Client::hasReceivedBotJoin() const {
	return (received_botjoin);
}


// Recibir datos del cliente
bool Client::receiveData(Server &server) {
    char temp_buffer[512];
    int bytes_received = recv(socket_fd, temp_buffer, sizeof(temp_buffer) - 1, 0);

    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            std::cout << "Client disconnected." << std::endl;
        } else {
            std::cout << "Error receiving data from client." << std::endl;
        }
        return false;
    }
    temp_buffer[bytes_received] = '\0';
	std::cout << "Datos recibidos: " << temp_buffer << std::endl;
    buffer.append(temp_buffer);
    size_t pos;
    while ((pos = buffer.find("\r\n")) != std::string::npos) {
        std::string command = buffer.substr(0, pos);
		processCommand(command, *this, server);
        buffer.erase(0, pos + 2);
    }
    return true;
}

void Client::sendResponse(const std::string& response) {
    std::string response_with_crlf = convertToCRLF(response);
    if (send(socket_fd, response_with_crlf.c_str(), response_with_crlf.size(), 0) == -1) {
        std::cout << "Error: Failed to send response to client." << std::endl;
    }
}

