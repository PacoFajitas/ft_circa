/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pongCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlopez-i <mlopez-i@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:47:38 by mlopez-i          #+#    #+#             */
/*   Updated: 2024/10/29 16:53:13 by mlopez-i         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

void	handlePongCommand(const std::vector<std::string>& tokens) 
{
    if (tokens.size() < 2) 
    {
        return;  // Si el PONG no tiene parámetros, lo ignoramos
    }
}
