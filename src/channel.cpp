#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"
#include "utils.hpp"

//El modo +o no es un modo de canal es para dar o quitar permisos de operador a un cliente en concreto dentro de cada canal
Channel::Channel(const std::string& channel_name, Server& srv, Client &client) : name(channel_name), server(srv) 
{
	std::cout << "Canal creado: " << name << std::endl;
    manageUser(&client, OPERATOR, true);
    modes['i'] = false;
    modes['t'] = false;
    modes['k'] = false;
    modes['l'] = false;
    topic = "";
    password = "";
    limitUsers = 0;
    //poner los modos a false:D
}

// GETERS AND SETERS

int	Channel::getLimitUsers() const
{
    return(limitUsers);
}

const	std::string	Channel::getPassword() const
{
    return(password);
}

// Returns the name of the channel
const	std::string& Channel::getName() const 
{
		return (name);
}

// Returns if the selected mode is active or not
const	bool& Channel::getMode(char mode) const 
{
    std::map<char, bool>::const_iterator iter = modes.find(mode);
    return iter->second;
}

//returns the flags of all the activated modes in a string
//Example: +ik
const	std::string Channel::getAllModes() const 
{
    std::string ret = "+";
    std::stringstream str;
    str << limitUsers;
    std::string limitstr = str.str();
    if(modes.at('i') == true)
        ret += 'i';
    if (modes.at('t') == true)
        ret += 't';
    if (modes.at('l') == true)
        ret += 'l';
    if (modes.at('k') == true)
        ret += 'k';
    if (modes.at('l') == true)
    {
        ret += " ";
        ret += limitstr;
    }
    if (modes.at('k') == true)
        ret += " " + (this->getPassword());
    return (ret);
}

// Returns the map with Clients* and UserRole
const std::map<Client *, UserRole> Channel::getUsers() const
{
    return users;
}

//Devuelve true o false dependiendo si el nick passado por params esta en el canal
bool	Channel::isUserInChannel(std::string nick)
{
    std::map<Client *, UserRole>::iterator it;
    it = users.begin();
    while (it != users.end())
    {
        if(it->first->getNickname() == nick)
            return true;
        ++it;
    }
    return false;    
}

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
        return (concatenateVectors(ret, ret2));
    }
    else if (mode == "ALL")
    {
        std::vector<Client *> ret2;
        ret = getUsersWithRole("INCHANNEL");
        ret2 = getUsersWithRole("INVITED");
        return (concatenateVectors(ret, ret2));
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
    printClientVector(ret);
    return (ret);
}
//Estas dos funciones se pueden juntar y hacer una generica 
//para saber si cualquier usuario pertenece a cualquier rol
bool	Channel::isUserRole(const Client& client, std::string role)
{
    std::vector<Client *>aux;
    aux = getUsersWithRole(role);
    if (aux.empty())
        return (false);
    std::vector<Client *>::iterator it = aux.begin();
    while (it != aux.end())
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
void    Channel::setMode(char mode, bool active)
{
    if (active == false && getMode(mode) == false)
        return ;
    if (active == true && getMode(mode) == true)
        return ;
    if (active)
    {
        if (mode == 'i')
            modes['i'] = true;
        else if (mode == 't')
            modes['t'] = true;
    }
    else
    {
        if (mode == 'i')
            modes['i'] = false;
        else if (mode == 't')
            modes['t'] = false;
        else if (mode == 'k')
            modes['k'] = false;
        else if (mode == 'l')
        {
            // std::cout << "tus muertos pisados" << std::endl;
            modes['l'] = false;
        }
        
    }
    
}


void    Channel::setMode(char mode, bool active, std::string msg)
{
    if (active == false && getMode(mode) == false)
        return ;
    if (active == true && getMode(mode) == true)
        return ;
    if (active == true)
    {
        if (mode == 'k')
        {
            modes['k'] = true;
            password = msg;
        }
        else if (mode == 'l')
        {
            modes['l'] = true;
            limitUsers = atoi(msg.c_str());
        }
        else if(mode == 'o')
        {
            this->setRole(msg, OPERATOR);
        }
    }
    else
    {
         if (mode == 'k')
            modes['k'] = false;
        else if (mode == 'l')
            modes['l'] = false; 
        else if(mode == 'o')
        {
            this->setRole(msg, PARTICIPANT);
        }
    }
}


Client	*Channel::getUser(std::string user)
{
    std::map<Client *, UserRole>::iterator it = users.begin();
    while(it != users.end())
    {
        if (it->first->getUsername() == user)
            return(it->first);
        it++;  
    }
    return NULL;
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
//     }
// }
bool Channel::hasSentModeToClient(const Client& client) {
    return clientsWithModeSent.find(client.getSocketFD()) != clientsWithModeSent.end();
}

void Channel::setModeSentToClient(const Client& client) {
    clientsWithModeSent.insert(client.getSocketFD());
}

