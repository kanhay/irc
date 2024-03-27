/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 16:26:14 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/27 14:23:13 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server(){}
Server::~Server(){}

void    Server::errorMessage(std::string msg);

bool    Server::isInUseNickname(std::string nickname){
    if (clients.find(nickname) != clients.end())
        return true;
    return false;
}
bool    Server::isInUseUsername(std::string username){
    if (clients.find(username) != clients.end())
        return true;
    return false;
}

void    Server::addClient(Client const& client){
    clients[client.getNickname()] = client;
}

bool    Server::isInUseChName(std::string chName){
    
}
void    Server::addChannel(Channel const& channel){
    channels[channel.getName()] = channel;
}