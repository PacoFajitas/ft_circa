/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topicCommand.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:35:11 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/24 16:35:43 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef TOPICCOMMAND_HPP
# define TOPICCOMMAND_HPP

#include "client.hpp"
#include "server.hpp"
#include "responses.hpp"
#include "channel.hpp"

void handleTopicCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif