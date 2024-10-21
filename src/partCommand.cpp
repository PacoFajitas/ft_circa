/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   partCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:27:07 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/21 16:57:57 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "partCommand.hpp"
#include "utils.hpp"

void handlePartCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
	if (tokens.size() < 2)
		server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "PART"));
	std::string err;
	Channel *channel = server.getChannel(tokens[1]);
	if (!channel)
		err = ERR_NOSUCHCHANNEL(tokens[1]);
	else if (!channel->isUserRole(client, "INCHANNEL"))
		err = ERR_NOTONCHANNEL(server.getServerName(), channel->getName());
	
	if (!err.empty())
	{
		server.sendResponse(client.getSocketFD(), err);
		return;
	}
	channel->manageUser(channel->getUser(client.getNickname()), PARTICIPANT, false);
	
	printClientVector(channel->getUsersWithRole("INCHANNEL"));
	
	std::string partNotification = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " PART " + channel->getName();

	std::string resp = ":" + client.getNickname() + " PART " + channel->getName();
	if (tokens.size() > 2)
	{
		resp += " :";
		for (uint i = 2; i < tokens.size() ; i++)
		{
			resp += tokens[i];
		}
	}
	server.sendResponse(client.getSocketFD(), partNotification);
	if (channel->getUsers().empty())
		server.deleteChannel(channel->getName());
	else
		channel->sendMessage(partNotification, client.getSocketFD());
}
