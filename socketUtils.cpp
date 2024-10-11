// socketUtils.cpp
#include "socketUtils.hpp"

// Crear el socket del servidor. Tipo ipv4, tipo stream, eleccion al sistema (TCP)
int	createSocket() {
	int	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		std::cout << "Error: Failed to create socket." << std::endl;
		exit(EXIT_FAILURE);
	}
	return (socket_fd);
}

//Funcion para reutilizar el address que utiliza el socket rapidamente, ahorrara tiempo al
//abrir y cerrar el programa
void	enableReuseAddress(int socket_fd) {
	int reuseAddrOn = 1; //// es como decir que estas siguientes opciones = on.
	 // setsockopt permite configurar muchas cosas del socket. Pero SOL_SOCKET es un nivel general.
	 // para acceder a SO_REUSEADDR tenemos que estar en nivel SOL_SOCKET. Despues el puntero y el size.
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddrOn, sizeof(reuseAddrOn)) == -1) {
		std::cout << "Error: Failed to set socket options for address reuse." << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

// Configurar el socket como no bloqueante. fcntl = file control sobre fd. 
// F_SETFL = Set File Status Flags. O_NONBLOCK = Non-blocking Mode
void	setNonBlockingMode(int socket_fd) {
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) {
		std::cout << "Error: Failed to set socket to non-blocking mode." << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
		}
}


// Configurar la estructura de la dirección del servidor. 
// Creamos una estructura sockaddr_in, que recogera la informacion del socket del servidor
void	bindSocket(int socket_fd, int port) {
	struct	sockaddr_in server_info;
	std::memset(&server_info, 0, sizeof(server_info));
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = INADDR_ANY;
	server_info.sin_port = htons(port);

	//socket_fd va asociado a un socket. Con el cast hacemos que server_info sea
	//tratado como un struct sockaddr. Con el bind, hacemos que socket_server_fd este
	////asociado con la dirección y el puerto especificados en server_info
	if (bind(socket_fd, (struct sockaddr*)&server_info, sizeof(server_info)) == -1) {
		std::cout << "Error: Failed to bind socket to port " << port << "." << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

// Poner el socket en modo escucha. Maxima cola de 10
void	listenOnSocket(int socket_fd) {
	if (listen(socket_fd, 10) == -1) {
		std::cout << "Error: Failed to listen on socket." << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

// Definición de la función para añadir un pollfd al vector
// pollfd es una estructura que se usa con la funcion poll() para supervisar
// últiples descriptores de archivos (como sockets) y detecta eventos de interés.
void	addPollFd(std::vector<pollfd>& poll_fds, int fd, short events) {
	struct	pollfd pfd; // Creamos una estgructurra llamada pfd del tipo pollfd 
	pfd.fd = fd;        // Si ponemos una estructura x tipo pollfd, la terminacion .fd (x.fd),
						// le estamos diciendo que queremos supervisar ese fd
	pfd.events = events;  // Le decimos que sea sensible a cualquier cosa de events 
						  // (como POLLIN, POLLOUT, POLLERR, etc.)
	poll_fds.push_back(pfd);  // Añadimos pfd al conjunto de Descriptores Supervisados (poll_fds), 
							  // que es un vector.
}
