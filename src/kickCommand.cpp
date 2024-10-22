/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 17:38:48 by tfiguero          #+#    #+#             */
/*   Updated: 2024/10/22 20:28:46 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kickCommand.hpp"

void handleKickCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
	if (tokens.size() < 3)
	{
		server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
		return ;
	}
	Channel *channel = server.getChannel(tokens[1]);
	std::string err;
	if (!channel)
		err = ERR_NOSUCHCHANNEL(channel->getName());
	else if (!channel->isUserRole(client, "INCHANNEL"))
		err = ERR_NOTONCHANNEL(server.getServerName(), channel->getName());
	else if (!channel->isUserRole(client, "OPERATOR"))
		err = ERR_CHANOPRIVSNEEDED(server.getServerName(), channel->getName());
	else if (!channel->isUserInChannel(tokens[2]))
		err = ERR_USERNOTINCHANNEL(server.getServerName(), tokens[2], channel->getName());
	if(!err.empty())
	{
		server.sendResponse(client.getSocketFD(),err);
		return ;
	}
	
	std::string resp = "";
	if (tokens.size() > 2)
	{
		resp = " :";
		for (uint i = 2; i < tokens.size() ; i++)
		{
			if (i != 2)
				resp += " ";
			resp += tokens[i];
		}
	}
	server.sendResponse(channel->getUser(tokens[2])->getSocketFD(), RPL_KICKPART(client.getNickname(), client.getUsername(), client.getHostname(), 
		channel->getName(), " KICK ", tokens[2], resp));
	channel->manageUser(channel->getUser(tokens[2]), PARTICIPANT, false);
	if (channel->getUsers().empty())
	{
		server.deleteChannel(channel->getName());
		return ;
	}
	channel->sendMessage(RPL_KICKPART(client.getNickname(), client.getUsername(), client.getHostname(), 
		channel->getName(), " KICK ", tokens[2], resp), -1);
	std::string clientList = channel->clientOpList();
	if (!clientList.empty())
    {
        channel->sendMessage(RPL_NAMREPLY(server.getServerName(), client.getNickname(), channel->getName(), clientList), -1);
        channel->sendMessage(RPL_ENDOFNAMES(client.getNickname(),channel->getName()), -1);
    }
}