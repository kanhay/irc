/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 13:41:41 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/27 14:45:28 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

void isValidNickName(std::string nickname){
    if (!nickname.empty()){
        for (unsigned int i = 0; i < nickname.size(); i++){
            if (!isdigit(nickname[i] && nickname)
        }
    }
    else
        std::cerr << "error: empty nickname\n";exit(1);
}

void isValidUserName(std::string username){
    if (!username.empty()){
        
    }
    else
        std::cerr << "error: empty username\n";
}
Client::Client(Server server, std::string nn, std::string un, std::string hn, std::string rn, int sd)
:hostname(hn), realname(rn), servername("ft_irc"),
registred(1), socket(sd){
    isValidNickName(nn);
    isValidUserName(un);
    if (!server.isInUseNickname(nickname))
        nickname = nn;
    else
        std::cerr << "error: nickname already in use\n"; exit(1);
    if (!server.isInUseUsername(username))
        username = un;
    else
        std::cerr << "error: username already in use\n"; exit(1);
    server.addClient(*this); //add the new client to the server container if successfully created
}
Client::~Client(){}

//getters
std::string Client::getNickname() const{
    return (nickname);
}
std::string Client::getUsername() const{
    return (username);
}
std::string Client::getHostname() const{
    return (hostname);
}
std::string Client::getRealname() const{
    return (realname);
}
int Client::getSocketDescriptor() const{
    return (socket);
}
bool Client::isRegistred() const{
    return (registred);
} 


//setters
void Client::setNickname(std::string newNickname){
    nickname = newNickname;
}
void Client::setSocketDescriptor(int sd){
    socket = sd;
}
void Client::setRegistered(bool b){
    registred = b;
}


//others
void Client::joinchannel(Channel &channel);