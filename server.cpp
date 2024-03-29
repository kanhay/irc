/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 16:26:14 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/28 21:22:10 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server(){}
Server::~Server(){
    //close users fd before quitting 
    for (unsigned int i = 0; i < clients.size(); i++)
        close(clients[i].getSocketDescriptor());
}

bool    Server::isInUseNickname(std::string nickname){
    for (unsigned int i = 0; i < clients.size(); i++){
        if (clients[i].getNickname() == nickname)
            return true;
    }
    return false;
}

void    Server::addUser(Client const& client){
    clients.push_back(client);
}

bool    Server::isInUseChName(std::string chName){
    for (unsigned int i = 0; i < channels.size(); i++){
        if (channels[i].getName() == chName)
            return true;
    }
    return false;
}
void    Server::addChannel(Channel const& channel){
    channels.push_back(channel);
}

void    Server::removeChannel(Channel const& channel){
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++){
        if (it->getName() == channel.getName()){
            channels.erase(it);
            break ;
        }
    }
}

void    Server::removeUser(Client const& client){
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++){
        if (it->getNickname() == client.getNickname()){
            clients.erase(it);
            break ;
        }
    }
}


