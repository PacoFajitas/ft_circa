/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsgCommand.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfiguero < tfiguero@student.42barcelona    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 13:35:11 by tfiguero          #+#    #+#             */
/*   Updated: 2024/10/21 13:36:05 by tfiguero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRIVMSGCOMMAND_HPP
# define PRIVMSGCOMMAND_HPP

void handlePrivmsgCommand(Client& client, const std::vector<std::string>& tokens, Server& server);
#endif