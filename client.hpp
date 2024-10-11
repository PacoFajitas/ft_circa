// client.hpp
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include "server.hpp"

class Client {
private:
    int socket_fd;                 // Descriptor de archivo del socket del cliente
    std::string nickname;          // Nickname del cliente
    std::string username;          // Username del cliente
    std::string realname;          // Realname del cliente
	std::string hostname;		   // hostname del cliente (lo usaremos mucho como referencia)
    bool	authenticated;            // Estado de autenticación del cliente
    bool	registered;               // Indica si el cliente ha completado la autenticación
	bool    received_welcome;
    std::string buffer;            // Buffer para datos entrantes, manejar datos parciales

public:
    // Constructor
    Client(int fd);

    // Getters y Setters
    int getSocketFD() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
	const std::string& getHostname() const;
    bool	isAuthenticated() const;
    bool	isRegistered() const;
	bool	isFullyRegistered() const;
	bool	canBeRegistered() const;

	bool	getHasPassword() const;

    void	setNickname(const std::string& nick);
    void	setUsername(const std::string& user);
    void	setRealname(const std::string& real);
	void	setHostname(const std::string& hostname);
    void	setAuthenticated(bool state);
    void	setRegistered(bool state);
	void	setReceivedWelcome(bool status);
	bool	hasReceivedWelcome() const;

    // Manejo de datos y comandos
	bool receiveData(Server& server);
//    void handleCommand(const std::string& command);  // Procesar comando recibido
    void sendResponse(const std::string& response);  // Enviar respuestas al cliente
};

#endif // CLIENT_HPP
