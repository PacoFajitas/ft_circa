#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

Channel::Channel(const std::string& channel_name, Server& srv, Client &client) : name(channel_name), server(srv) 
{
	std::cout << "Canal creado: " << name << std::endl;
    manageUser(&client, OPERATOR, true);
    _modes['i'] = false;
    _modes['t'] = false;
    _modes['k'] = false;
    _modes['o'] = false;
    _modes['l'] = false;
    //poner los modos a false:D
}

// GETERS AND SETERS

// Returns the name of the channel
const	std::string& Channel::getName() const 
{
		return (name);
}

// Returns if the selected mode is active or not
const	bool& Channel::getMode(char mode) const 
{
    std::map<char, bool>::const_iterator iter = _modes.find(mode);
    return iter->second;
}

// Returns the map with Clients* and UserRole
const std::map<Client *, UserRole> Channel::getUsers() const
{
    return users;
}

// void printClientVector(const std::vector<Client *>& vec) {
//     std::cout << "Client Vector Size: " << vec.size() << " Client Vector: ";
//     for (size_t i = 0; i < vec.size(); ++i) {
//         std::cout << vec[i]->getNickname() << " ";
//     }
//     std::cout << " |"<< std::endl; // Para terminar la línea después de imprimir
// }

// Returns a vector of Clients* with the selected role (PARTICIPANT, OPERATOR, INVITED, INCHANNEL, ALL)
std::vector<Client *> Channel::getUsersWithRole(std::string mode)
{
    std::vector<Client *> ret;
    UserRole tag;

    if (mode == "INCHANNEL")
    {
        std::vector<Client *> ret2;
        ret = getUsersWithRole("PARTICIPANT");
        ret2 = getUsersWithRole("OPERATOR");
        if (ret.size() > 0 && ret2.size() > 0)
            ret.insert(ret.end(), ret2.begin(), ret2.end());
        if (ret2.size() > 0)
            return (ret2);
        return (ret);
    }
    else if (mode == "ALL")
    {
        std::vector<Client *> ret2;
        ret = getUsersWithRole("INCHANNEL");
        ret2 = getUsersWithRole("INVITED");
        if (ret.size() > 0 && ret2.size() > 0)
            ret.insert(ret.end(), ret2.begin(), ret2.end());
        if (ret2.size() > 0)
            return (ret2);
        return (ret);
    }

    if (mode == "PARTICIPANT")
        tag = PARTICIPANT;
    else if (mode == "OPERATOR")
        tag = OPERATOR;
    else if (mode == "INVITED")
        tag = INVITED;

   
    std::map<Client *, UserRole>::iterator it;
    it = users.begin();
    while (it != users.end())
    {
        if (it->second == tag)
            ret.push_back(it->first);
        ++it;
    }
    // printClientVector(ret);
    return (ret);
}

bool Channel::getClientInvited(Client &client)
{
    std::vector<Client *> ret;
    ret = getUsersWithRole("INVITED");
    if (ret.empty())
        return (false);
    std::vector<Client *>::iterator it = ret.begin();
    while (it != ret.end())
    {
        if ((*it)->getNickname() == client.getNickname())
            return (true);
        ++it;
    }
    return (false);
}

//Returns all ClientFDs in a vector
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

// Returns the topic of the channel
const	std::string& Channel::getTopic() const 
{ 
    return topic; 
}

// Sets the topic of the channel
void	Channel::setTopic(const std::string& newTopic) 
{ 
    topic = newTopic; 
}

// Sets the role of a user
void Channel::setRole(std::string user,  UserRole rol)
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

// METHODS

// Adds/Deletes a Client from the users map
void	Channel::manageUser(Client* client, UserRole role, bool add) {

    if (add)
        users[client] = role;
    else
    {
        users.erase(client);
    }

}

void	Channel::dummyUseServer() {
    std::cout << &server << std::endl;
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

