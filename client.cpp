/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 13:41:41 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/30 13:39:41 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <cctype> //tolower fun
#include <sstream>

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


void    nickCommand(std::string &args, Server &s, Client &c){
    std::istringstream iss(args);
    std::string nn;
    if (!args.empty() && iss >> nn && iss.eof()){
        tolowercase(nn); //Nicknames are generally case-insensitive
        if (!isValidNickName(nn))
            sendMsg(c.getSocketDescriptor(), "Erroneous Nickname\n");
        if (s.isInUseNickname(nn))
            sendMsg(c.getSocketDescriptor(), "Nickname is in use.\n");
        c.setNickname(nn); //in both case: choosing a nname for the first time or changing a nname 
    }
    else
        std::cerr << "Not enough parameters.\n"; exit (1);
    c.registerClient();//client become registred in the server if the condition inside registerClient is true
}

void    userCommand(std::string &args, Client &c, Server &s){
    std::istringstream iss(args);
    std::string un, hn, sn, rn;
    iss >> un >> hn >> sn;
    iss >>std::ws;
    if (iss.peek() == ':'){
        std::string restOfStrem;
        getline(iss, restOfStrem);
        restOfStrem = restOfStrem.substr(restOfStrem.find_first_not_of(": "));
        rn = restOfStrem.substr(0, restOfStrem.find_first_of(':'));
    }
    else
        iss >> rn;
    if (!iss.fail() && !un.empty() && !hn.empty() && !sn.empty() && !rn.empty()){
        c.setUsername(un);
        c.setHostname(hn);
        c.setServername(sn);
        c.setRealname(rn);
        std::cout << "i am printing in userCommand:\n" << "un = " << un << "hn = " << hn << "sn = " << sn << "rn = " << rn << "\n";
    }
    else
        sendMsg(c.getSocketDescriptor(), "Not enough parameters.\n");
    c.registerClient();//client become registred in the server if the condition inside registerClient is true
}

void    passCommand(std::string &args, Client &c, Server &s){
    if (!c.isRegistered() && !c.isPasswordSended()){ //if the client is not yet registred and didn't give password before
        std::istringstream iss(args);
        std::string param;
        if (iss >> param && iss.eof()){
            param = param.substr(param.find_first_not_of(": ")); //skip : and space
            if (param != s.getPassword())
                sendMsg(c.getSocketDescriptor(), "Incorrect password.\n");
            c.setPasswordSended(true);  //leave a mark if pass cmd succeed the first time
        }
        else
            sendMsg(c.getSocketDescriptor(), "Invalid syntax for user command.\n");
    }
    else if ((!c.isRegistered() && c.isPasswordSended())) //in case a password has already been set but the client attempts to send the PASS command multiple times during the connection process
        sendMsg(c.getSocketDescriptor(), "PASS command should only be sent once.\n");
    else
        sendMsg(c.getSocketDescriptor(), "You may not reregister.\n"); //the client is already registred
    c.registerClient();//client become registred in the server if the condition inside registerClient is true
}

Client::Client(){
    nickname = username = hostname = servername = realname = "\0";
    registered = PasswordSended = false;
    socket = -1;
}
Client::~Client(){
    //colse client's sochet
    server.removeUser(*this);
    PasswordSended = false; //to reautenticate the next time he connect to the server
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
std::string Client::getServername() const{
    return (servername);
}
int Client::getSocketDescriptor() const{
    return (socket);
}
bool Client::isRegistered() const{
    return (registered);
} 
bool    Client::isPasswordSended(){
    return true;
}

//setters
void Client::setNickname(std::string nn){
    nickname = nn;
}
void Client::setUsername(std::string un){
    username = un;
}
void Client::setHostname(std::string hn){
    hostname = hn;
}
void Client::setServername(std::string sn){
    servername = sn;
}
void Client::setRealname(std::string rn){
    realname = rn;
}
void Client::setSocketDescriptor(int sd){
    socket = sd;
}
void Client::setRegistered(bool b){
    registered = b;
}

void Client::setPasswordSended(bool b){
    PasswordSended = b;
}


//others
void Client::joinchannel(Channel &channel);

void Client::registerClient(){
    if (PasswordSended && !nickname.empty() && !username.empty()
    && !hostname.empty() && !servername.empty() && !realname.empty())
        registered = true;
}

