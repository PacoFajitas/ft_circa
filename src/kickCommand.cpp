/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 17:38:48 by tfiguero          #+#    #+#             */
/*   Updated: 2024/10/21 13:05:48 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kickCommand.hpp"

void handleKickCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
	if (tokens.size() < 3)
		server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
	Channel *channel = server.getChannel(tokens[1]);
	std::string error;
	if (!channel)
		error = ERR_NOSUCHCHANNEL(channel->getName());
	else if (!channel->isUserRole(client, "INCHANNEL"))
		error = ERR_NOTONCHANNEL(server.getServerName(), channel->getName());
	else if (!channel->isUserRole(client, "OPERATOR"))
		error = ERR_CHANOPRIVSNEEDED(server.getServerName(), channel->getName());
	else if (!channel->isUserInChannel(tokens[2]))
		error = ERR_USERNOTINCHANNEL(server.getServerName(), tokens[2], channel->getName());
	if(!error.empty())
	{
		server.sendResponse(client.getSocketFD(),error);
		return ;
	}
	else
	{
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
}