#ifndef RESPONSES_HPP
#define RESPONSES_HPP

// NICKNAME
#define ERR_ERRONEUSNICKNAME(nickname) (std::string("432 ") + nickname + " :Erroneous nickname")
#define ERR_NICKNAMEINUSE(nickname) (std::string("433 ") + nickname + " :Nickname is already in use")
#define ERR_NONICKNAMEGIVEN() (std::string("431 :No nickname given"))
#define ERR_NICKCOLLISION(nickname) (std::string("436 ") + nickname + " :Nickname collision KILL")
#define RPL_NICKCHANGE(oldnick, user, host, nick)(std::string(":") + oldnick + "!" + user + "@" + host + " NICK :" + nick)
#define RPL_NICKUSER(oldnick, newnick) (std::string (":" + oldnick + " NICK " + newnick))

// USER
#define ERR_NEEDMOREPARAMS(client, command) (std::string("461 ") + (client) + " " +(command) + " :Not enough parameters")
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
#define RPL_JOIN(nickname, username, hostname, channel) \
    (std::string(":" + nickname + "!" + username + "@" + hostname + " JOIN :" + channel))
#define RPL_NAMREPLY(servername, nickname, channel, clientlist) \
    (std::string(":" + servername + " 353 " + nickname + " = " + channel + " :" + clientlist))
#define RPL_ENDOFNAMES(nickname, channel) (std::string(":" + server.getServerName() + " 366 " + nickname + " " + channel + " :End of /NAMES list"))

// PRIVMSG
#define ERR_NORECIPIENT(servername) (std::string(":" + servername + " 411 :No recipient given (PRIVMSG)"))
#define ERR_NOTEXTTOSEND(servername) (std::string(":" + servername + " 412 :No text to send"))
#define ERR_CANNOTSENDTOCHAN(servername, channel) (std::string(":" + servername + " 404 " + channel + " :Cannot send to channel"))
#define ERR_NOSUCHNICK(servername, nickname) (std::string(":" + servername + " 401 " + nickname + " :No such nick/channel"))
#define RPL_PRIVMSG(sender, receiver, message) (std::string(":" + sender + " PRIVMSG " + receiver + " " + message))

// PASSWORD
#define ERR_PASSWDMISMATCH() (std::string("464 :Password incorrect"))


// PART
#define ERR_USERNOTINCHANNEL(servername, nickname, channel) (std::string(":" + servername + " 441 " + nickname + " " + channel + " :They aren't on that channel"))
#define ERR_NOTONCHANNEL(servername, channel) (std::string(":" + servername + " 442 " + channel + " :You're not on that channel"))

// MODE
#define RPL_CHANNELMODEIS(servername, nickname, channel, flag) \
    (std::string(":" + servername + " 324 " + nickname + " " + channel + " " + flag))
#define ERR_KEYSET(servername, channel) (std::string(":" + servername + " 467 " + channel + " :Channel key already set"))
#define ERR_UNKNOWNMODE(servername, user, flag) (std::string(":" + servername + " " + user + " 472 " + flag + " :is unknown mode char to me for "))
#define RPL_OPERATOR(client, username, hostname, channel, active, nick) (std::string (":" + client + "!" + username + "@" + hostname + " MODE " + active + "o " + channel + " " + nick))
// #define ERR_UMODEUNKNOWNFLAG(client)(std::string ("501: ") + client + " :Unknown MODE flag")

// :quien_invita!usuario@host INVITE tu_nick :#canal
// INVITE
#define ERR_USERONCHANNEL(servername, nickname, channel) \
    (std::string(":" + servername + " 443 " + nickname + " " + channel + " :is already on channel"))
#define ERR_CHANOPRIVSNEEDED(servername, channel) \
    (std::string(":" + servername + " 482 " + channel + " :You're not channel operator"))
#define RPL_INVITING(servername, nickname, channel, nicknameinvited) \
    (std::string(":" + servername + " 341 " + nickname + " " + channel + " " + nicknameinvited))
#define RPL_YOUVEBEENINVITED(client, username, hostname, invited, channel) \
    (std::string (":" + client + "!" + username + "@" + hostname + " INVITE " + invited + " " + channel))

// PING
#define RPL_PONG(servername, text) (std::string(":" + servername + " PONG " + text))

// QUIT
#define ERR_NORECIPIENTQUIT(servername) (std::string(":" + servername + " 431 :No recipient given (QUIT)"))
#define RPL_QUIT(nick, user, hostname, message)(std::string(":" + nick+ "!" + user + "@" + hostname + " QUIT :" + message))

// KICK
#define ERR_USERNOTINCHANNELKICK(servername, nickname, channel) (std::string(":" + servername + " 441 " + nickname + " " + channel + " :They aren't on that channel"))
#define RPL_KICKPART(client, username, hostname, channel, kickpart, nick, reason)(std::string (":" + client + "!" + username + "@" + hostname + kickpart + channel + " " + nick + reason))

// GENERAL
#define ERR_UNKNOWNCOMMAND(command) (std::string("421 " + command + " :Unknown command"))
#define ERR_NOTREGISTERED() (std::string("451 :You have not registered"))  // Error si mandamos algo sin estar registrados

// WHO
#define RPL_WHOREPLY(server, client, channel, username, hostname, nick, op, realname) \
    (std::string (":" + server + " 352 " + client + " " + channel + " " + username + " " + hostname + " " + server + " " + nick + " H" + op + " :0" + realname))
#define RPL_ENDOFWHO(server, client, channel) \
    (std::string (":" + server + " 315 " + client + " " + channel + " :End of /WHO list."))

// TOPIC
#define RPL_NOTOPIC(servername, channel) (std::string(":" + servername + " 331 " + channel + " :No topic is set"))
#define RPL_TOPIC(servername, client, channel, topic) (std::string(":" + servername + " 332 " + client + " " + channel + " " + topic))
#define RPL_CHANGETOPIC(client, user, host, channel, topic) (std::string (":" + client + "!" + user + "@" + host + " TOPIC " + channel + " " + topic))


#endif // RESPONSES_HPP
