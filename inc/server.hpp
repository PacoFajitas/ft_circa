#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <set>


class Channel;
class Client;
class FileTransfer;
class Server {
private:
    int socket_fd;
    int port;
    std::string password;
    std::string serverName;
    std::string serverTime;
    std::vector<pollfd> poll_fds;
    std::map<int, Client*> clients;
	std::map<std::string, Channel*> channels;
    std::map<int, FileTransfer*> activeTransfers;

    void setupSignalHandler();
    void pollSockets();
    void handlePollEvents();
    void acceptClient();
	bool nickUsed(std::string nickname);
	void newClient(int client_fd);
    void handleClient(int client_fd);
    void handleWrite(int client_fd); // Añadido
    void cleanup();
    void setTime();

    // Funciones de manejo de comandos
    bool handlePassCommand(Client& client, const std::vector<std::string>& tokens);
    bool handleUserCommand(Client& client, const std::vector<std::string>& tokens);
    bool handlePingCommand(Client& client, const std::vector<std::string>& tokens);
//	void sendMessage(int client_fd, const std::string& message);


public:
    Server(); // Constructor sin parámetros
    void	start();
    void	run();
	void	configureServer(int port, std::string password);
	std::string	getTime() const;
	void	sendResponse(int client_fd, const std::string& message);
	std::string	getServerName() const;
	std::string	getPassword() const;
	void broadcastMessage(const std::string& message, const std::set<int>& include_fds, const std::set<int>& exclude_fds);
    void deleteChannel(std::string name);

	const	std::map<int, Client*>& getClients() const;
	Channel* getChannel(const std::string& channel_name);
	void addChannel(Channel* channel);
	void    sendPing(Client& client);
	std::vector<Client*> getChannelUsers(const std::string& channelName);
    void handleAcceptFile(Client& client, const std::vector<std::string>& tokens);
    void handleRejectFile(Client& client, const std::vector<std::string>& tokens);
    void addFileTransfer(Client* sender, Client* recipient, const std::string& fileName, std::streamsize fileSize);
    std::map<int, FileTransfer*>& getActiveTransfers();
    FileTransfer* getFileTransfer(int clientFD);
    void removeFileTransfer(int recipientFD);
    Client* getClientByNick(const std::string& nickname) const;
    void sendBinaryData(int client_fd, const std::vector<char>& data);
    void handleFileSendRequest(Client& client, const std::vector<std::string>& tokens);
    void broadcastMessage(const std::string& message, const std::set<int>& include_fds);

	~Server();



};

#endif // SERVER_HPP

