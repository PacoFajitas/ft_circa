/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   partCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:27:07 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/17 19:17:40 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "partCommand.hpp"

void handlePartCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
	if (tokens.size() < 2)
		server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
	Channel *channel = server.getChannel(tokens[1]);
	if (!channel)
		server.sendResponse(client.getSocketFD(), "ERR_NOSUCHCHANNEL (403) <client> <channel> :No such channel");
	else if (!channel->isUserRole(client, "INCHANNEL"))
		server.sendResponse(client.getSocketFD(), "ERR_NOTONCHANNEL (442) <client> <channel> :You're not on that channel");
	else
	{
		channel->manageUser(channel->getUser(tokens[2]), PARTICIPANT, false);
		std::string resp = ":" + client.getNickname() + " PART " + channel->getName();
		if (tokens.size() > 2)
		{
			resp += " :";
			for (uint i = 2; i < tokens.size() ; i++)
			{
				resp += tokens[i];
			}
		}
		// std::vector<Client*>::iterator it = channel->getUsersWithRole("INCHANNEL").begin();
		// while (it != channel->getUsersWithRole("INCHANNEL").end())
		// {
		// 	server.sendResponse((*it)->getSocketFD(), resp);
		// 	++it;
		// }
	}
}