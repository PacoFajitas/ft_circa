/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inviteCommand.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 17:30:52 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/30 18:16:45 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

void handleInviteCommand(Client& client, const std::vector<std::string>& tokens, Server& server)
{
	if (tokens.size() < 3)
	{
		server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS(client.getNickname(), "INVITE"));
		return ;
	}
	Channel *channel = server.getChannel(tokens[2]);
	Client* invitedclient = server.getClientByNick(tokens[1]);
	std::string err;
	if (!channel)
		err = ERR_NOSUCHCHANNEL(tokens[2]);
	else if (!channel->isUserInChannel(client.getNickname()))
		err = ERR_NOTONCHANNEL(server.getServerName(), channel->getName());
	else if (!channel->isUserRole(client, "OPERATOR"))
		err = ERR_CHANOPRIVSNEEDED(server.getServerName(), channel->getName());
	else if (!invitedclient)
		err = ERR_NOSUCHNICK(server.getServerName(), tokens[1]);
	else if (channel->isUserInChannel(invitedclient->getNickname()))
		err = ERR_USERONCHANNEL(server.getServerName(), invitedclient->getNickname(), channel->getName());
	if (!err.empty())
	{
		server.sendResponse(client.getSocketFD(), err);
		return ;
	}
	channel->manageUser(invitedclient, INVITED, true);
	server.sendResponse(client.getSocketFD(), RPL_INVITING(server.getServerName(),
		client.getNickname(), channel->getName(), invitedclient->getNickname()));
	server.sendResponse(invitedclient->getSocketFD(),RPL_YOUVEBEENINVITED(client.getNickname(), client.getUsername(), 
		client.getHostname(), invitedclient->getNickname(), channel->getName()));
}
