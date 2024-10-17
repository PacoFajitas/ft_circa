/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 17:38:48 by tfiguero          #+#    #+#             */
/*   Updated: 2024/10/17 18:31:58 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kickCommand.hpp"

void handleKickCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
	if (tokens.size() < 3)
		server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
	Channel *channel = server.getChannel(tokens[1]);
	if (!channel)
		server.sendResponse(client.getSocketFD(), "ERR_NOSUCHCHANNEL (403) <client> <channel> :No such channel");
	else if (!channel->isUserRole(client, "INCHANNEL"))
		server.sendResponse(client.getSocketFD(), "ERR_NOTONCHANNEL (442) <client> <channel> :You're not on that channel");
	else if (!channel->isUserRole(client, "OPERATOR"))
		server.sendResponse(client.getSocketFD(), "ERR_CHANOPRIVSNEEDED (482) <client> <channel> :You're not channel operator");
	else if (!channel->isUserInChannel(tokens[2]))
		server.sendResponse(client.getSocketFD(), "ERR_USERNOTINCHANNEL (441) <client> <nick> <channel> :They aren't on that channel");
	else
	{
		//no puede ser token[2] pq necesita ser de tipo cliente. hacer un geter para esto que devuelva el obj cliente segun el nick que entre por parametro
		std::string resp = "User: " + tokens[2] + " has been kicked from channel: " + channel->getName();
		channel->manageUser(channel->getUser(tokens[2]), PARTICIPANT, false);
		std::vector<Client*>::iterator it = channel->getUsersWithRole("INCHANNEL").begin();
		while (it != channel->getUsersWithRole("INCHANNEL").end())
		{
			server.sendResponse((*it)->getSocketFD(), resp);
			++it;
		}
	}
}