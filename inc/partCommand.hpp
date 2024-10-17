/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   partCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:27:43 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/17 18:29:45 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARTCOMMAND_HPP
# define PARTCOMMAND_HPP

#include "client.hpp"
#include "server.hpp"
#include "responses.hpp"
#include "channel.hpp"

void handlePartCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif