#ifndef RESPONSES_HPP
#define RESPONSES_HPP

// NICKNAME
#define ERR_ERRONEUSNICKNAME(nickname) (std::string("432 ") + nickname + " :Erroneous nickname")
#define ERR_NICKNAMEINUSE(nickname) (std::string("433 ") + nickname + " :Nickname is already in use")
#define ERR_NONICKNAMEGIVEN() (std::string("431 :No nickname given"))
#define ERR_NICKCOLLISION(nickname) (std::string("436 ") + nickname + " :Nickname collision KILL")

// USER
#define ERR_NEEDMOREPARAMS(command) (std::string("461 ") + (command) + " :Not enough parameters")
#define ERR_ALREADYREGISTRED() (std::string("462 :You may not reregister"))

// WELCOME
#define RPL_WELCOME(nickname, servername, hostname) \
    (std::string(":" + servername + " 001 " + nickname + \
    " :Welcome to the " + servername + " Network, " + nickname + "!" + hostname))
#define RPL_YOURHOST(servername, nickname) \
    (std::string(":" + servername + " 002 " + nickname + " Your host is " + servername + ", running version 1.0"))
#define RPL_CREATED(servername, time) \
    (std::string(":" + servername + " 003 This server was created " + time))
#define RPL_MYINFO(servername, nickname) \
    (std::string(":" + servername + " 004 " + nickname + " " + servername))


// JOIN
#define ERR_NOSUCHCHANNEL(channel) (std::string("403 ") + channel + " :No such channel")
#define ERR_CHANNELISFULL(nickname, channel) (std::string(":server_name 471 " + nickname + " " + channel + " :Cannot join channel (+l)"))
#define ERR_INVITEONLYCHAN(channel) (std::string("473 " + channel + " :Cannot join channel (+i)"))
#define ERR_BADCHANNELKEY(nickname, channel) (std::string("475 " + nickname + " " + channel + " :Cannot join channel (+k)"))
#define RPL_NOTOPIC(servername, channel) (std::string(":" + servername + " 331 " + channel + " :No topic is set"))
#define RPL_TOPIC(servername, channel, topic) (std::string(":" + servername + " 332 " + channel + " :" + topic))
#define RPL_NAMREPLY(servername, nickname, channel, clientlist) \
    (std::string(":" + servername + " 353 " + nickname + " = " + channel + " :" + clientlist))
#define RPL_ENDOFNAMES(nickname, channel) (std::string(":" + server.getServerName() + " 366 " + nickname + " " + channel + " :End of /NAMES list"))

// PRIVMSG
#define ERR_NORECIPIENT(servername) (std::string(":" + servername + " 411 :No recipient given (PRIVMSG)"))
#define ERR_NOTEXTTOSEND(servername) (std::string(":" + servername + " 412 :No text to send"))
#define ERR_CANNOTSENDTOCHAN(servername, channel) (std::string(":" + servername + " 404 " + channel + " :Cannot send to channel"))
#define ERR_NOSUCHNICK(servername, nickname) (std::string(":" + servername + " 401 " + nickname + " :No such nick/channel"))


// PASSWORD
#define ERR_PASSWDMISMATCH() (std::string("464 :Password incorrect"))


// PART
#define ERR_USERNOTINCHANNEL(servername, nickname, channel) (std::string(":" + servername + " 441 " + nickname + " " + channel + " :They aren't on that channel"))
#define ERR_NOTONCHANNEL(servername, channel) (std::string(":" + servername + " 442 " + channel + " :You're not on that channel"))

// MODE
#define RPL_CHANNELMODEIS(servername, nickname, channel, flag) \
    (std::string(":" + servername + " 324 " + nickname + " " + channel + " " + flag))
#define ERR_KEYSET(servername, channel) (std::string(":" + servername + " 467 " + channel + " :Channel key already set"))
#define ERR_UNKNOWNMODE(servername, channel, flag) (std::string(":" + servername + " 472 " + flag + " :is unknown mode char to me for " + channel))

// INVITE
#define ERR_USERONCHANNEL(servername, nickname, channel) \
    (std::string(":" + servername + " 443 " + nickname + " " + channel + " :is already on channel"))
#define ERR_CHANOPRIVSNEEDED(servername, channel) \
    (std::string(":" + servername + " 482 " + channel + " :You're not channel operator"))
#define RPL_INVITING(servername, nickname, channel, nicknameinvited) \
    (std::string(":" + servername + " 341 " + nickname + " " + channel + " " + nicknameinvited))

// PING
#define RPL_PONG(servername, text) (std::string(":" + servername + " PONG " + text))

// QUIT
#define ERR_NORECIPIENTQUIT(servername) (std::string(":" + servername + " 431 :No recipient given (QUIT)"))

// KICK
#define ERR_USERNOTINCHANNELKICK(servername, nickname, channel) (std::string(":" + servername + " 441 " + nickname + " " + channel + " :They aren't on that channel"))

// GENERAL
#define ERR_UNKNOWNCOMMAND(command) (std::string("421 " + command + " :Unknown command"))
#define ERR_NOTREGISTERED() (std::string("451 :You have not registered"))  // Error si mandamos algo sin estar registrados

#endif // RESPONSES_HPP

