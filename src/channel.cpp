#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"
#include "utils.hpp"


Channel::Channel(const std::string& channel_name, Server& srv, Client &client, Client *bot) : name(channel_name), server(srv), _bot(bot)
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
}

int	Channel::getLimitUsers() const
{
    return(limitUsers);
}

const	std::string	Channel::getPassword() const
{
    return(password);
}

const	std::string& Channel::getName() const 
{
		return (name);
}

const	bool& Channel::getMode(char mode) const 
{
    std::map<char, bool>::const_iterator iter = modes.find(mode);
    return iter->second;
}

// Returns the flags of all the activated modes in a string
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
    return (ret);
}

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
void Channel::setRole(std::string nick,  UserRole rol)
{
    std::map<Client *, UserRole>::iterator it = users.begin();
    while(it != users.end())
    {
        if (it->first->getNickname() == nick)
        {
            it->second = rol;
            return ;
        }
        it++;  
    }
    std::map<int, Client*> temp = server.getClients();
    std::map<int, Client*>::iterator it2 = temp.begin();
    while(it2 != temp.end())
    {
        
        if(it2->second->getNickname() == nick)
            break ;
    }
    if (it2 == temp.end())
    {
        ERR_NOSUCHNICK(server.getServerName(), nick);
        return ;
    }
    if(it == users.end())
        ERR_USERNOTINCHANNEL(server.getServerName(), nick, this->name);
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
            modes['l'] = false;
        }
        
    }
    
}


void    Channel::setMode(char mode, bool active, std::string msg)
{
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
            if (isUserInChannel(msg))
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
            this->setRole(msg, PARTICIPANT);
    }
}


Client	*Channel::getUser(std::string nick)
{
    std::map<Client *, UserRole>::iterator it = users.begin();
    while(it != users.end())
    {
        if (it->first->getNickname() == nick)
            return(it->first);
        it++;  
    }
    return NULL;
}

Client *Channel::getBot()
{
	return _bot;
}

// Adds/Deletes a Client from the users map
void	Channel::manageUser(Client* client, UserRole role, bool add) {

    if (add)
        users[client] = role;
    else
        users.erase(client);
}

void	Channel::dummyUseServer() {
    std::cout << &server << std::endl;
}

bool Channel::hasSentModeToClient(const Client& client) {
    return clientsWithModeSent.find(client.getSocketFD()) != clientsWithModeSent.end();
}

void Channel::setModeSentToClient(const Client& client) {
    clientsWithModeSent.insert(client.getSocketFD());
}

// Sends a message to everyone except _fd, -1 to send to everyone in channel
void    Channel::sendMessage(const std::string& message, const int _fd) {
    std::vector<Client *> tmp = getUsersWithRole("INCHANNEL");
    for (std::vector<Client *>::const_iterator it = tmp.begin(); it != tmp.end(); ++it) 
    {
        int fd = (*it)->getSocketFD();
        if (fd != _fd)
            server.sendResponse(fd, message);
    }
}

std::string Channel::clientOpList()
{
    std::string clientList;
    const std::vector<Client*>& participants = getUsersWithRole("INCHANNEL");
    for (size_t i = 0; i < participants.size(); ++i) {
        if (isUserRole(*participants[i], "OPERATOR"))
            clientList += "@";
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    }
    return clientList;
}

