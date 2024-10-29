/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topicCommand.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:34:24 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/29 16:51:38 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

void handleTopicCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
	if (tokens.size() == 1)
	{
		server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "TOPIC"));
		return;
	}
	Channel *channel = server.getChannel(tokens[1]);
	std::string err;
	if (!channel)
		err = ERR_NOSUCHCHANNEL(tokens[1]);
	else if (!channel->isUserRole(client, "INCHANNEL"))
		err = ERR_NOTONCHANNEL(server.getServerName(), channel->getName());
	else if (tokens.size() == 2)
	{
		if (channel->getTopic().empty())
			err = RPL_NOTOPIC(server.getServerName(), channel->getName());
		else
			err = RPL_TOPIC(server.getServerName(), client.getNickname(), channel->getName(), channel->getTopic());
	}
	else if (channel->getMode('t') && !channel->isUserRole(client, "OPERATOR"))
		err = ERR_CHANOPRIVSNEEDED(server.getServerName(), channel->getName());
	if (!err.empty())
	{
		server.sendResponse(client.getSocketFD(), err);
		return ;
	}
	std::string newtopic = "";
	for (size_t i = 2; i < tokens.size(); i++)
	{
		if (i != 2)
			newtopic += " ";
		newtopic += tokens[i];
	}
	channel->setTopic(newtopic);
	channel->sendMessage(RPL_CHANGETOPIC(client.getNickname(), client.getUsername(), client.getHostname(), channel->getName(), channel->getTopic()), -1);
}