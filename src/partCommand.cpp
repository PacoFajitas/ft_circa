/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   partCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:27:07 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/22 18:58:46 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "partCommand.hpp"
#include "utils.hpp"

void handlePartCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
	if (tokens.size() < 2)
	{
		server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "PART"));
		return ;
	}
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
	// std::cout << "Printing Client Vector before deleting user" << std::endl;
	// printClientVector(channel->getUsersWithRole("INCHANNEL"));
	// std::cout << "Printing Client Vector after deleting user" << std::endl;
	// printClientVector(channel->getUsersWithRole("INCHANNEL"));
	std::string resp = " ";
	if (tokens.size() > 2)
	{
		resp = " :";
		for (uint i = 2; i < tokens.size() ; i++)
		{
			resp += tokens[i];
		}
	}
	server.sendResponse(client.getSocketFD(), RPL_KICKPART(client.getNickname(), client.getUsername(), client.getHostname(), 
		channel->getName(), " PART ", client.getNickname(), resp));
	channel->manageUser(channel->getUser(client.getNickname()), PARTICIPANT, false);
	if (channel->getUsers().empty())
	{
		server.deleteChannel(channel->getName());
		return ;
	}
	channel->sendMessage(RPL_KICKPART(client.getNickname(), client.getUsername(), client.getHostname(), 
		channel->getName(), " PART ", client.getNickname(), resp), -1);
	std::string clientList = channel->clientOpList();
	if (!clientList.empty())
    {
        channel->sendMessage(RPL_NAMREPLY(server.getServerName(), client.getNickname(), channel->getName(), clientList), -1);
        channel->sendMessage(RPL_ENDOFNAMES(client.getNickname(),channel->getName()), -1);
    }
}
