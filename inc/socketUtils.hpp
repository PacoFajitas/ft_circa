// socketUtils.hpp
#ifndef SOCKETUTILS_HPP
#define SOCKETUTILS_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>

// Funciones para la configuración de sockets
int		createSocket();
void	enableReuseAddress(int socket_fd);
void	setNonBlockingMode(int socket_fd);
void	bindSocket(int socket_fd, int port);
void	listenOnSocket(int socket_fd);

void	addPollFd(std::vector<pollfd>& poll_fds, int fd, short events);

#endif // SOCKETUTILS_HPP

