/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 17:23:49 by tfiguero          #+#    #+#             */
/*   Updated: 2024/10/31 17:31:30 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"
#include "responses.hpp"
#include "channel.hpp"

void    Server::initializeBot()
{
    _bot = new Client(-1);
    _bot->setRealname("Wall-E•ᴗ•");
    _bot->setNickname("Wall-E•ᴗ•");
    _bot->setHostname("8.8.8.8");
    _bot->setUsername("Wall-E•ᴗ•");
    _bot->setAuthenticated(true);
    _bot->setRegistered(true);
}

void Server::sendBotWelcome(int client, Server &server, std::string nick)
{
	sendResponse(client, RPL_PRIVMSG(server.getBot()->getNickname(), nick, "          | Hi!! |"));
	sendResponse(client, RPL_PRIVMSG(server.getBot()->getNickname(), nick, "    /)  /)_   ___|"));
	sendResponse(client, RPL_PRIVMSG(server.getBot()->getNickname(), nick, "   (• ^ •) \\/"));
	sendResponse(client, RPL_PRIVMSG(server.getBot()->getNickname(), nick," </       \\>  "));
	sendResponse(client, RPL_PRIVMSG(server.getBot()->getNickname(), nick, "To join a channel you can type JOIN <channel_name> or /join <channel_name> if you are using HexChat!"));
}

void Server::sendBotJoin(Client *client, Channel *channel)
{
	sendResponse(client->getSocketFD(), RPL_PRIVMSG(channel->getBot()->getNickname(), client->getNickname(), "If you are a channel operator you can use: MODE (itkol), INVITE, TOPIC or KICK."));
	sendResponse(client->getSocketFD(), RPL_PRIVMSG(channel->getBot()->getNickname(), client->getNickname(), "If you are a normal user (you peasant...), just enjoy chatting with PRIVMSG <channel or nick> and the message or type on your selected channel (Hexchat only)."));
}