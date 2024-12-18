/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:59:23 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/11/03 19:39:46 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <csignal>
#include <ctime>  // Incluir para manejo del tiempo
#include <poll.h>
#include <algorithm> // para std::remove_if
#include "server.hpp" // Asegurarse de que el encabezado esté correctamente incluido
#include "socketUtils.hpp"
#include "utils.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "cscCommand.hpp"
#include "fileTransfer.hpp"

bool Server::isValidNickChan(std::string str, bool ischan)
{
    if (str.empty())
        return false;
    if (!ischan && (str.at(0) == '#' || str.at(0) == '&'))
        return false;
    if (ischan && str.at(0) != '#' && str.at(0) != '&')
        return false;
    if (ischan && str.size() < 2)
        return false;
    std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-[]\\`^{}|_";
    if (ischan && str.substr(1).find_first_not_of(validChars) != std::string::npos)
        return false;
    if (!ischan && str.find_first_not_of(validChars) != std::string::npos)
        return false;
    if (str == "KICK" || str == "INVITE" || str == "MODE" || str == "JOIN" || str == "TOPIC" || str == "PASS" || str == "NICK" || str == "USER"
        || str == "PING" || str == "PONG" || str == "PART" || str == "PRIVMSG" || str == "WHO" || str == "NAMES")
        return false;
    return true;
}

//Volatile para que no cachee sino que la consulte siempre
//sig_atomic_t tipo de C y CPP para variables que pueden modificadarse en un manejador de señales,
//ya que es un tipo atomico, cualquier operacion se hara de una forma indivisible (un golpe)
//stop_server lo vamos a usar como flag para ver cuando para el servidor
volatile sig_atomic_t stop_server = 0;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        stop_server = 1;
    }
}


// Función para configurar los manejadores de señales
void	Server::setupSignalHandler() {
    std::signal(SIGINT, signalHandler);   // Manejar interrupción Ctrl+C
    std::signal(SIGTERM, signalHandler);  // Manejar solicitud de terminación
    std::signal(SIGPIPE, SIG_IGN);        // Ignorar SIGPIPE para evitar que el servidor se cierre por desconexiones
}

void Server::setTime() {
    std::time_t now = std::time(NULL); 
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    serverTime = buffer;
}

std::string Server::getTime() const {
	return (serverTime);
}

Server::Server(void) {
	this->serverName = "MyIRCserver";
	this->setTime();
}

/***********
// Método de configuración del manejador de señales en la clase Server
void	Server::setupSignalHandler() {
	std::signal(SIGINT, signalHandler);
}
**********/
void	Server::configureServer(int port, std::string password) {
	this->password = password;
	this->port = port;

	socket_fd = createSocket();
	enableReuseAddress(socket_fd);
	setNonBlockingMode(socket_fd);
	bindSocket(socket_fd, port);
	listenOnSocket(socket_fd);
    initializeBot();
	addPollFd(poll_fds, socket_fd, POLLIN);
    
	setupSignalHandler();
}

Server::~Server() {};

void	Server::run() {
	setupSignalHandler();

	int prevClientCount = poll_fds.size() - 1;

	while (!stop_server) {
		int currentClientCount = poll_fds.size() - 1;
		if (prevClientCount != currentClientCount) {
			std::cout << "Currently monitoring " << currentClientCount << " clients." << std::endl;
			prevClientCount = currentClientCount;
		}
        pollSockets();
        handlePollEvents();
    }

    cleanup();
}
// Supervisamos los sockets utilizando la llamada al sistema poll()
// poll_fds.data(): Devuelve un puntero al primer elemento del vector poll_fds, 
// que contiene estructuras pollfd para cada socket que el servidor está supervisando.
// -1: Especifica que poll() debe esperar indefinidamente hasta que ocurra un evento 
// en uno de los descriptores de archivo supervisados.
void Server::pollSockets() {
    int poll_count = poll(poll_fds.data(), poll_fds.size(), -1); //el poll_fds

    if (poll_count < 0) {
        if (stop_server)
            return;
    std::cout << "Error: Poll failed." << std::endl;
    stop_server = 1;
    }
}

// Encabezado de la función para manejar los eventos de poll
void Server::handlePollEvents() {
    for (size_t i = 0; i < poll_fds.size(); ++i) {
        if (poll_fds[i].revents & POLLIN) {
            if (poll_fds[i].fd == socket_fd) {
                acceptClient();
            } else {
                handleClient(poll_fds[i].fd);
            }
        }
		if (poll_fds[i].revents & POLLOUT) {
			handleWrite(poll_fds[i].fd);
    	}
	}
}

void Server::acceptClient() {
    sockaddr_in temp;
    socklen_t temp_len;
    temp_len = sizeof(temp);
    int client_fd = accept(socket_fd, (struct sockaddr *)&temp, &temp_len);
    if (client_fd < 0) {
        std::cout << "Error: Failed to accept new client." << std::endl;
        return;
    }
    const char* ipStr = inet_ntoa(temp.sin_addr);
    std::cout << ipStr << std::endl;
    // Añadimos el cliente
	newClient(client_fd, ipStr);
}

// Función para gestionar un nuevo cliente
void Server::newClient(int client_fd, const char* hostname) {
    Client* new_client = new Client(client_fd);
    //almacena la información de la dirección IP y el puerto del cliente
    new_client->setConnected(true);
    new_client->setHostname(hostname);
	clients[client_fd] = new_client;
    addPollFd(poll_fds, client_fd, POLLIN);
	std::cout << "New client connected: " << client_fd << std::endl;
}
bool Server::nickUsed(std::string nickname)
{
	std::map<int, Client*>::iterator it;
	it = clients.begin();
	while (it != clients.end())
	{
		if (it->second->getNickname() == nickname)
			return true;
	}
	return false;
}

// Manejar la comunicación con los clientes
void Server::handleClient(int client_fd)
{
	Client* client = clients[client_fd];
    if (!client->receiveData(*this)) {  // Si falla la recepción de datos, desconectar
        std::cout << "Client disconnected: " << client_fd << std::endl;
        close(client_fd);
		disconnectClient(client, "Client disconnected");
    }
}

//Funcion que envia el mensaje de desconexion de usuario a todos los canales en los que estaba
//y si el canal se queda vacio lo elimina y libera tanto el canal como el cliente
//cerrando y eliminando el socket correspondiente
void Server::disconnectClient(Client* client, const std::string& quitMessage) 
{
    int client_fd = client->getSocketFD();
    
    std::string message = RPL_QUIT(client->getNickname(), client->getUsername(), client->getHostname(), quitMessage);
    std::vector<Channel *> usrChanns = getChannelsFromClient(*client);
    std::vector<Channel *>::iterator it = usrChanns.begin();
    while (it != usrChanns.end()) {
        Channel* channel = *it;
        channel->sendMessage(message, client_fd);
        channel->manageUser(client, PARTICIPANT, false);
        if (channel->getUsers().empty()) {
            std::string channelName = channel->getName();
            deleteChannel(channelName);
        }
        ++it;
    }
    clients.erase(client_fd);
    // Cerrar y eliminar el socket del cliente
    close(client_fd);
    std::vector<pollfd>::iterator pollIt = poll_fds.begin();
    while (pollIt != poll_fds.end()) {
        if (pollIt->fd == client_fd) {
            pollIt = poll_fds.erase(pollIt);
        } else {
            ++pollIt;
        }
    }
    delete client;
}

// Retorna el nombre del servidor configurado
std::string Server::getServerName() const {
    return (serverName);
}

// Manejar la escritura de datos a los clientes (placeholder, por si se necesita)
void Server::handleWrite(int client_fd) {
	(void)client_fd;
}

void Server::sendResponse(int client_fd, const std::string& response) {
    std::string response_with_crlf = convertToCRLF(response); // Asegura \r\n

    if (send(client_fd, response_with_crlf.c_str(), response_with_crlf.size(), 0) == -1) {
        std::cerr << "Error: Failed to send response to client (fd " << client_fd << ")." << std::endl;
    }
}
//se podria hacer un template que recorra un mapa y aplique la funcion que le entra por parametro al mapa;
void	Server::broadcastMessage(const std::string& message, 
			const std::set<int>& include_fds = std::set<int>(), 
			const std::set<int>& exclude_fds = std::set<int>()) {
        // Si include_fds está vacío, enviaremos a todos los clientes.
	if (include_fds.empty()) {
		for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
			int fd = it->first;
			// Si fd no está en exclude_fds, enviamos el mensaje.
			if (exclude_fds.find(fd) == exclude_fds.end()) {
				sendResponse(fd, message);
			}
		}
	}
	else {
		// Si include_fds no está vacío, solo enviamos a esos fds.
		for (std::set<int>::const_iterator it = include_fds.begin(); it != include_fds.end(); ++it) {
			int fd = *it;
			// Si fd no está en exclude_fds, enviamos el mensaje.
			if (exclude_fds.find(fd) == exclude_fds.end()) {
				sendResponse(fd, message);
			}
		}
	}
}

void	Server::broadcastMessage(const std::string& message, 
	const std::set<int>& include_fds = std::set<int>()) {
        // Si include_fds está vacío, enviaremos a todos los clientes.
	if (include_fds.empty()) {
		for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
			int fd = it->first;
			// Si fd no está en exclude_fds, enviamos el mensaje.
			sendResponse(fd, message);
		}
	}
	else {
		// Si include_fds no está vacío, solo enviamos a esos fds.
		for (std::set<int>::const_iterator it = include_fds.begin(); it != include_fds.end(); ++it) {
			int fd = *it;
			// Si fd no está en exclude_fds, enviamos el mensaje.
				sendResponse(fd, message);
		}
	}
}

void Server::deleteChannel(std::string name)
{
    delete channels[name];
    channels.erase(name);
}

const std::map<int, Client*>& Server::getClients() const {
	return (clients); // 'clients' es el nombre de la variable que almacena los clientes.
}

void Server::cleanup() {
    std::cout << "Signal detected, cleaning up..." << std::endl;
    delete _bot;
    // Cerrar todos los descriptores de archivo supervisados
    for (size_t i = 0; i < poll_fds.size(); ++i) {
        close(poll_fds[i].fd);
    }
    poll_fds.clear();

    // Liberar memoria de los clientes y limpiar el mapa
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        delete it->second;
    }
    clients.clear();

	// Liberar memoria de los canales y limpiar el mapa
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		delete it->second;
		it->second = NULL;
		std::cout << "Erasing channel: " << it->first << std::endl;
	}
    for (std::map<int, FileTransfer*>::iterator it = activeTransfers.begin(); it != activeTransfers.end(); ++it) {
		delete it->second;
	}
	activeTransfers.clear();

    // Cerrar el socket del servidor
    close(socket_fd);
   	std::cout << "Server shutdown completed." << std::endl;
}


std::string Server::getPassword() const {
	return (password); // Devuelve la contraseña almacenada en la clase Server
}

Client *Server::getBot()
{
	return _bot;
}

Channel* Server::getChannel(const std::string& channel_name) {
    std::map<std::string, Channel*>::iterator it = channels.find(channel_name);
    if (it != channels.end()) {
        return it->second;
    }
    return NULL; // Retorna NULL si el canal no existe
}

std::vector<Channel *> Server::getChannelsFromClient(Client &user)
{
    std::vector<Channel *> ret;
    for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if(it->second->isUserInChannel(user.getNickname()))
            ret.push_back(it->second);
    }
    
	return (ret);
}

void	Server::addChannel(Channel* channel) {
    channels[channel->getName()] = channel; // Añade el canal al mapa
}

void	Server::sendPing(Client& client) {
	std::string pingMsg = "PING :" + getServerName();
	sendResponse(client.getSocketFD(), pingMsg);
}

std::vector<Client*> Server::getChannelUsers(const std::string& channelName) {
    Channel* channel = getChannel(channelName);
    if (channel) {
        return (channel->getUsersWithRole("INCHANNEL"));
    }
    return std::vector<Client*>();
}


void Server::handleFileSendRequest(Client& client, const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        sendResponse(client.getSocketFD(), "Error: Incorrect file send request format.");
        return;
    }

    std::string recipientNick = tokens[1];
    std::string fileName = tokens[2];
	
	if (fileName.size() > 255) {
		fileName = fileName.substr(0, 255);
	}


    Client* recipient = getClientByNick(recipientNick);
    if (!recipient) {
        sendResponse(client.getSocketFD(), ERR_NOSUCHNICK(getServerName(), recipientNick));
        return;
    }

    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::ate);
    if (!file) {
        sendResponse(client.getSocketFD(), "Error: File not found or not accessible.");
        return;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

	std::string shortFileName;
	size_t lastSlashPos = fileName.find_last_of("/");
	if (lastSlashPos != std::string::npos) {
		shortFileName = fileName.substr(lastSlashPos + 1);
		}
	else {
        shortFileName = fileName;
    }

	std::ostringstream oss;
	oss << fileSize;
	std::string fileSizeStr = oss.str();

	std::string notification = "PRIVMSG " + recipient->getNickname() + " :" +
		client.getNickname() + 
		" wants to send you the file: " + shortFileName + "\n" +
		"Size: " + fileSizeStr + " bytes\nAccept by typing: /ACCEPT_F " + shortFileName +
		"\nReject by typing: /REJECT_F " + shortFileName;	
	sendResponse(recipient->getSocketFD(), notification);

	std::string senderNotification = "PRIVMSG " + client.getNickname() + 
		" :Attempting to send the file " +
        shortFileName + " to " + recipient->getNickname() + "...";
    sendResponse(client.getSocketFD(), senderNotification);

    addFileTransfer(&client, recipient, fileName, fileSize);
}

// Aqui manejamos la aceptacion del archivo por parte del cliente receiver
void Server::handleAcceptFile(Client& client, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        sendResponse(client.getSocketFD(), "Error: Missing file name in the acceptance request.");
        return;
    }

    std::string fileName = tokens[1];
    int clientFD = client.getSocketFD();

    // Verificamos si hay una solicitud de archivo activa
    if (getActiveTransfers().find(clientFD) == getActiveTransfers().end()) {
        sendResponse(client.getSocketFD(), "Error: No pending file transfer requests.");
        return;
    }

    FileTransfer* transfer = getActiveTransfers()[clientFD];

    std::ostringstream oss;
    oss << transfer->getFileSize();

    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        sendResponse(client.getSocketFD(), "Error: Could not determine user's home directory.");
        return;
    }

    std::string downloadPath = std::string(homeDir) + "/Downloads";

    // Creamos el directorio si no exite
    struct stat st;
	if (stat(downloadPath.c_str(), &st) != 0) {
		// Intentamos crear la carpeta si no existe
		if (mkdir(downloadPath.c_str(), 0777) != 0) {
			sendResponse(client.getSocketFD(), "Error: Failed to create directory 'DescargasChat' on the Desktop.");
        return;
	    }
	}
    downloadPath += "/" + fileName;

	std::string senderNotification = "PRIVMSG " + transfer->getSender()->getNickname() +
		" :Your file " + fileName + " has been accepted. The transfer will begin shortly.";
	sendResponse(transfer->getSender()->getSocketFD(), senderNotification);

	std::string receiverNotification = "PRIVMSG " + transfer->getRecipient()->getNickname() +
		" :You have accepted the file " + fileName + ". The transfer will begin shortly.";
	sendResponse(transfer->getRecipient()->getSocketFD(), receiverNotification);

	sendFileData(*(transfer->getSender()), *(transfer->getRecipient()), fileName, downloadPath, *this);

}

// Ahora el rechazo de la transferencia por parte del cliente receiver
void Server::handleRejectFile(Client& client, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        sendResponse(client.getSocketFD(), "Error: Missing file name in the rejection request.");
        return;
    }

    std::string fileName = tokens[1];
    int clientFD = client.getSocketFD();

    // Verificamos si hay una solicitud de archivo activa
    if (getActiveTransfers().find(clientFD) != getActiveTransfers().end()) {
        FileTransfer& transfer = *getActiveTransfers()[clientFD];
        
        // Notificamos al remitente que la solicitud fue rechazada y confirmamos al receiver que ha rechazado la transferencia
        sendResponse(transfer.getSender()->getSocketFD(),"The file transfer for " + fileName + " was rejected.");
        sendResponse(client.getSocketFD(), "You have rejected the file " + fileName + " transfer.");
        
        getActiveTransfers().erase(clientFD);
    }
}

void Server::addFileTransfer(Client* sender, Client* recipient, const std::string& fileName, std::streamsize fileSize) {
    int recipientFD = recipient->getSocketFD();
    FileTransfer* transfer = new FileTransfer(sender, recipient, fileName, fileSize); 
    activeTransfers[recipientFD] = transfer;
}

std::map<int, FileTransfer*>& Server::getActiveTransfers() {
	return (activeTransfers);
}

FileTransfer* Server::getFileTransfer(int clientFD) {
	if (activeTransfers.find(clientFD) != activeTransfers.end()) {
		return (activeTransfers[clientFD]);
	}
	return (NULL);
}

void Server::removeFileTransfer(int recipientFD) {
    if (activeTransfers.find(recipientFD) != activeTransfers.end()) {
        delete activeTransfers[recipientFD];
        activeTransfers.erase(recipientFD);
    }
}


Client* Server::getClientByNick(const std::string& nickname) const {
    std::map<int, Client*>::const_iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickname() == nickname) {
            return it->second;
        }
    }
    return NULL;
}

// Usamos std::ostringstream para construir el mensaje de error
void Server::sendBinaryData(int client_fd, const std::vector<char>& data) {
    std::ostringstream oss;
    oss << "Error: Failed to send binary data to client (fd " << client_fd << ").";
    std::string errorMsg = oss.str();

    if (send(client_fd, data.data(), data.size(), 0) == -1) {
        sendResponse(client_fd, errorMsg);
    }
}
