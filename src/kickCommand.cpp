/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 17:38:48 by tfiguero          #+#    #+#             */
/*   Updated: 2024/10/21 19:23:33 by mlopez-i         ###   ########.fr       */
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
	//no puede ser token[2] pq necesita ser de tipo cliente. hacer un geter para esto que devuelva el obj cliente segun el nick que entre por parametro
	std::string resp = "User: " + tokens[2] + " has been kicked from channel: " + channel->getName();
	channel->manageUser(channel->getUser(tokens[2]), PARTICIPANT, false);
	if(channel->getUsers().empty())
	{
		server.deleteChannel(channel->getName());
		return ;
	}
	std::vector<Client*>::iterator it = channel->getUsersWithRole("INCHANNEL").begin();
	while (it != channel->getUsersWithRole("INCHANNEL").end())
	{
		server.sendResponse((*it)->getSocketFD(), resp);
		++it;
	}
}