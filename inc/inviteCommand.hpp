/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inviteCommand.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 17:31:11 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/21 17:32:57 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INVITECOMMAND_HPP
# define INVITECOMMAND_HPP

#include "client.hpp"
#include "channel.hpp"
#include "server.hpp"
#include "responses.hpp"

void handleInviteCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif