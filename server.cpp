#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
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

//Volatile para que no cachee sino que la consulte siempre
//sig_atomic_t tipo de C y CPP para variables que pueden modificadarse en un manejador de señales,
//ya que es un tipo atomico, cualquier operacion se hara de una forma indivisible (un golpe)
//stop_server lo vamos a usar como flag para ver cuando para el servidor
volatile sig_atomic_t stop_server = 0;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        stop_server = 1; // Señal para detener el servidor de manera segura
    }
}


// Función para configurar los manejadores de señales
void	Server::setupSignalHandler() {
    std::signal(SIGINT, signalHandler);   // Manejar interrupción Ctrl+C
    std::signal(SIGTERM, signalHandler);  // Manejar solicitud de terminación
    std::signal(SIGPIPE, SIG_IGN);        // Ignorar SIGPIPE para evitar que el servidor se cierre por desconexiones
}


/******************
// Definición del manejador de señales
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        stop_server = 1; // Señal para detener el servidor de manera segura
    }
}
*********************/

void Server::setTime() {
    // Obtener el tiempo actual
    std::time_t now = std::time(NULL); 
    char buffer[100];
    // Formatear el tiempo como una cadena legible
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    serverTime = buffer;  // Guardar el tiempo formateado en serverTime
}

std::string Server::getTime() const {
	return (serverTime);  // Retorna el tiempo guardado o ajusta según sea necesario
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
	this->password = password;  // Asignar la contraseña
	this->port = port;          // Asignar el puerto

	socket_fd = createSocket();//Creamos un socket 
	enableReuseAddress(socket_fd);// Opcion de reusar rapidamente la direccion
	setNonBlockingMode(socket_fd);// Que no nos bloquee
	bindSocket(socket_fd, port);//Vinculamos el socket a la IP y puerto
	listenOnSocket(socket_fd);// Lo ponemos en escucha

	// Añadir el socket a la lista de descriptores supervisados por poll (abajo: sensible a POLLIN)
	addPollFd(poll_fds, socket_fd, POLLIN); //Añadir el socket_fd, a los poll_fds

	setupSignalHandler();      // Configurar el manejador de señales
}


Server::~Server() {};



void	Server::run() {
	setupSignalHandler();  // Configurar el manejador de señales

	int prevClientCount = poll_fds.size() - 1;

	while (!stop_server) {  // Repetir mientras no se haya recibido la señal de interrupción
		int currentClientCount = poll_fds.size() - 1;
		if (prevClientCount != currentClientCount) {
			std::cout << "Currently monitoring " << currentClientCount << " clients." << std::endl;
			prevClientCount = currentClientCount;
		}
        pollSockets();  // Realizar poll para supervisar los sockets
        handlePollEvents();  // Manejar los eventos de poll
    }

    cleanup();  // Limpieza de recursos al finalizar
}





// Supervisamos los sockets utilizando la llamada al sistema poll()
void Server::pollSockets() {
// poll_fds.data(): Devuelve un puntero al primer elemento del vector poll_fds, 
// que contiene estructuras pollfd para cada socket que el servidor está supervisando.
// -1: Especifica que poll() debe esperar indefinidamente hasta que ocurra un evento 
// en uno de los descriptores de archivo supervisados.
    int poll_count = poll(poll_fds.data(), poll_fds.size(), -1); //el poll_fds

    if (poll_count < 0) {// Error!
        if (stop_server) {  // Si la señal fue recibida, salir del bucle sin error
            return;
        }
        std::cout << "Error: Poll failed." << std::endl;
        stop_server = 1;  // Señal para detener el servidor en caso de error
    }
}





// Encabezado de la función para manejar los eventos de poll
void Server::handlePollEvents() {
    for (size_t i = 0; i < poll_fds.size(); ++i) {
        if (poll_fds[i].revents & POLLIN) {  // Hay datos disponibles para leer
            if (poll_fds[i].fd == socket_fd) {  // Actividad en el socket del servidor
                acceptClient();  // Aceptar nueva conexión de cliente
            } else {  // Actividad en un socket de cliente
                handleClient(poll_fds[i].fd);  // Manejar datos del cliente
            }
        }
		if (poll_fds[i].revents & POLLOUT) {  // Socket listo para enviar datos
			handleWrite(poll_fds[i].fd);  // Manejar la escritura de datos
    	}
	}
}

void Server::acceptClient() {
    int client_fd = accept(socket_fd, NULL, NULL);
    if (client_fd < 0) {
        std::cout << "Error: Failed to accept new client." << std::endl;
        return;
    }
    // Añadimos el cliente
	newClient(client_fd);
}

// Función para gestionar un nuevo cliente
void Server::newClient(int client_fd) {
    // Crear un nuevo cliente
    Client* new_client = new Client(client_fd);

    // Añadir el nuevo cliente al mapa de clientes
	clients[client_fd] = new_client;
    // Añadir el nuevo cliente a los descriptores supervisados por poll
    addPollFd(poll_fds, client_fd, POLLIN);

    // Mensaje de confirmación
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
        for (std::vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ) {
            if (it->fd == client_fd) {
                it = poll_fds.erase(it);
            } else
                ++it;
        }
        delete clients[client_fd];
        clients.erase(client_fd);
    }
}

// Retorna el nombre del servidor configurado
std::string Server::getServerName() const {
    return (serverName);
}

// Manejar la escritura de datos a los clientes (placeholder, por si se necesita)
void Server::handleWrite(int client_fd) {
	(void)client_fd; // Evitar la advertencia de parámetro no usado
    // Aquí podemos manejar la lógica de escritura si se requiere en el futuro
}




void Server::sendResponse(int client_fd, const std::string& response) {
    std::string response_with_crlf = convertToCRLF(response); // Asegura \r\n

    if (send(client_fd, response_with_crlf.c_str(), response_with_crlf.size(), 0) == -1) {
        std::cerr << "Error: Failed to send response to client (fd " << client_fd << ")." << std::endl;
    }
}





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











const std::map<int, Client*>& Server::getClients() const {
	return (clients); // 'clients' es el nombre de la variable que almacena los clientes.
}

void Server::cleanup() {
    std::cout << "Signal detected, cleaning up..." << std::endl;

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
    // Cerrar el socket del servidor
    close(socket_fd);
   	std::cout << "Server shutdown completed." << std::endl;
}


std::string Server::getPassword() const {
	return (password); // Devuelve la contraseña almacenada en la clase Server
}

Channel* Server::getChannel(const std::string& channel_name) {
    std::map<std::string, Channel*>::iterator it = channels.find(channel_name);
    if (it != channels.end()) {
        return it->second;
    }
    return NULL; // Retorna NULL si el canal no existe
}

void	Server::addChannel(Channel* channel) {
    channels[channel->getName()] = channel; // Añade el canal al mapa
}

void	Server::sendPing(Client& client) {
	// Crea el mensaje PING
	std::string pingMsg = "PING :" + getServerName();
	//Envia el mensaje PING al cliente
	sendResponse(client.getSocketFD(), pingMsg);
}

std::vector<Client*> Server::getChannelUsers(const std::string& channelName) {
    Channel* channel = getChannel(channelName);
    if (channel) {
        return (channel->getParticipants());  // Retorna los usuarios del canal si existe
    }
    return std::vector<Client*>();  // Si el canal no existe, devuelve un vector vacío
}

