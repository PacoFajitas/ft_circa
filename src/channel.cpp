#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

Channel::Channel(const std::string& channel_name, Server& srv, Client &client) : name(channel_name), server(srv) 
{
	std::cout << "Canal creado: " << name << std::endl;
    manageUser(&client, OPERATOR, true);
    //poner los modos a false:D
}

void	Channel::manageUser(Client* client, UserRole role, bool add) {

    if (add)
        users[client] = role;
    else
    {
        users.erase(client);
    }

}
//Funcion para obtener toda la lista de clientes con sus roles

const std::map<Client *, UserRole> Channel::getUsers() const
{
    return users;
}

//Funcion para setear el rol de un usuario en concreto
void Channel::setRol(std::string user,  UserRole rol)
{
    std::map<Client *, UserRole>::iterator it = users.begin();
    while(it != users.end())
    {
        if (it->first->getUsername() == user)
        {
            it->second = rol;
            return ;
        }
        it++;  
    }
    //En caso de que no lo encuentre en el canal busco tambien en el servidor para dar
    //El codigo de error que pertoque en cada situacion
    std::map<int, Client*> temp = server.getClients();
    std::map<int, Client*>::iterator it2 = temp.begin();
    while(it2 != temp.end())
    {
        
        if(it2->second->getUsername() == user)
            break ;
    }
    if (it2 == temp.end())
    {
        ERR_NOSUCHNICK(server.getServerName(), user);
        return ;
    }
    if(it == users.end())
        ERR_USERNOTINCHANNEL(server.getServerName(), user, this->name);
}
// Función para obtener el nombre del canal
const	std::string& Channel::getName() const {
		return (name);
}

std::vector<Client *> Channel::getUsersWithRole(std::string mode)
{
    std::vector<Client *> ret;
    if (mode == "INCHANNEL")
    {
        std::map<Client *, UserRole>::iterator it;
        it = users.begin();
        while (it != users.end())
        {
            if (it->second == PARTICIPANT || it->second == OPERATOR)
                ret.push_back(it->first);
            ++it;
        }
    }
}

//     // Función para obtener la lista de participantes
// const	std::vector<Client*>& Channel::getParticipants() const {
// 		return (participants);
// }

// const	std::vector<Client*>& Channel::getOperators() const {
// 	return (operators);
// }

// const	std::vector<Client*>& Channel::getInvited() const {
// 	return (invited);
// }

void	Channel::dummyUseServer() {
    std::cout << &server << std::endl;
}

//Devolver todos los fds de los clientes del canal en un vector de fds
std::set<int> Channel::getClientFDs() const {
    std::set<int> fds;
    std::map<Client *, UserRole>::const_iterator it = users.begin();
    while(it != users.end())
    {
        fds.insert(it->first->getSocketFD());
        ++it;  
    }
    return fds;
}


// En la clase Channel

// Almacena los clientes a los que ya se les ha enviado el modo
// void Channel::switchMode(std::string mode)
// {
//     if (mode)
//     {
//         ChannelPatata( )
//     }
    
// }
bool Channel::hasSentModeToClient(const Client& client) {
    return clientsWithModeSent.find(client.getSocketFD()) != clientsWithModeSent.end();
}

void Channel::setModeSentToClient(const Client& client) {
    clientsWithModeSent.insert(client.getSocketFD());
}

