/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kickCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 18:45:02 by tfiguero          #+#    #+#             */
/*   Updated: 2024/10/16 18:51:06 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KICKCOMMAND_HPP
# define KICKCOMMAND_HPP

#include "client.hpp"
#include "server.hpp"
#include "responses.hpp"
#include "channel.hpp"

void handleKickCommand(Client& client, const std::vector<std::string>& tokens, Server& server);
#endif