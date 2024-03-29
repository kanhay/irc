/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 13:41:41 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/28 22:42:52 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <cctype> //tolower fun

void    tolowercase(std::string &str){
    for (unsigned int i = 0; i < str.size(); i++)
        str[i] = std::tolower(str[i]);
}
bool isValidNickName(std::string nickname){
    if (!nickname.empty()){
        if (nickname.size() > 9)
            return false;//Nicknames can have a maximum length of 9 characters.
        for (unsigned int i = 0; i < nickname.size(); i++){
            if (!isdigit(nickname[i]) && nickname[i] != '-' && nickname[i] != '_' && nickname[i] != '\\'
            && !(nickname[i] >= 'a' && nickname[i] <= 'z')) //no need to check uppercase alphabets since nichname is already converted to lowercase in this step
                return false; //consist of letters (a-z, A-Z), numbers (0-9), and certain special characters, such as underscore (_), hyphen (-), and backslash ().
        }
        if (isdigit(nickname[0])) //The first character of a nickname must be a letter or a special character
            return false;
        if (nickname == "nickserv" || nickname == "chanserv") //are often reserved or restricted by the IRC server for other server services.
            return false ;
    }
    else
        return(0);//empty nickname
}

void    nickCommand(std::string &nn, Server &server, Client &client){
    tolowercase(nn); //Nicknames are generally case-insensitive
    if (!isValidNickName(nn))
        std::cout << "error: nickname wenti\n"; exit (1);
    if (server.isInUseNickname(nn))
        std::cerr << "error: nickname already in use\n"; exit(1);
    server.addUser(client); //add the new client to the server container if successfully created   
}

Client::Client(){}
Client::~Client(){
    //colse client's sochet
    server.removeUser(*this);
}

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
void Client::setNickname(std::string nn){
    nickname = nn;
}
void Client::setUsername(std::string un){
    username = un;
}
void Client::sethostname(std::string hn){
    hostname = hn;
}
void Client::setservername(std::string sn){
    servername = sn;
}
void Client::setrealname(std::string rn){
    realname = rn;
}
void Client::setSocketDescriptor(int sd){
    socket = sd;
}
void Client::setRegistered(bool b){
    registred = b;
}


//others
void Client::joinchannel(Channel &channel);

void Client::user(std::string un, std::string hn, std::string sn, std::string rn){
    setUsername(un);
    sethostname(hn);
    setservername(sn);
    setrealname(rn);
}