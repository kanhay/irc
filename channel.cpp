/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 16:19:20 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/27 17:07:39 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

Channel::Channel(Client const& creator, std::string chname, Server _server):name(chname), 
limit(30), topicLock(false), modeLock(false), server(_server){
    if (server.isInUseChName(chname))
        std::cerr << "error: there is an other channel with the same name\n";
    server.addChannel(*this); //add the new Channel in the server's channel list
    operators[creator.getNickname()] = creator; //the channel creator is considered an operator by default
}
Channel::~Channel(){
    server.removeChannel(*this);
}

//setters
void Channel::setMode(std::string newMode){
    mode = newMode;
}
void Channel::setTopic(std::string newTopic){
    topic = newTopic;
}
void Channel::setKey(std::string k){
    key = k;
}
void Channel::setModelock(bool b){
    modeLock = b;
}
void Channel::setTopiclock(bool b){
    topicLock = b;
}

//getters
std::string Channel::getName() const{
    return name;
}
std::string Channel::getTopic() const{
    return topic;
}
std::string Channel::getMode() const{
    return mode;
}
std::string Channel::getKey() const{
    return key;
}
int Channel::getlimit() const{
    return limit;
}
bool Channel::isModelocked() const{
    return modeLock;
}
bool Channel::isTopiclocked() const{
    return topicLock;
}
void Channel::addOperator(Client const& client){
    if (regularUsers.find(client.getNickname()) != regularUsers.end())
        regularUsers.erase(regularUsers.find(client.getNickname())); //remove client from regular users if exist before adding it to operators

    if (operators.find(client.getNickname()) == operators.end())
        operators[client.getNickname()] = client;
}
void Channel::removeOperator(Client const& client){
    if (operators.find(client.getNickname()) != operators.end())
        operators.erase(operators.find(client.getNickname()));
}

void Channel::addRegularUser(Client const& client){
    if (operators.find(client.getNickname()) != operators.end())
        operators.erase(operators.find(client.getNickname()));

    if (regularUsers.find(client.getNickname()) == regularUsers.end())
        regularUsers[client.getNickname()] = client;
}
void Channel::removeRegularUser(Client const& client){
    if (regularUsers.find(client.getNickname()) != regularUsers.end())
        regularUsers.erase(regularUsers.find(client.getNickname()));
}

bool Channel::isOperator(Client const& client) const{
    if (operators.find(client.getNickname()) != operators.end())
        return true;
    return false;
}
bool Channel::isRegularuser(Client const& client) const{
    if (regularUsers.find(client.getNickname()) != regularUsers.end())
        return true;
    return false;
}
bool Channel::isMember(Client const& client){
    if (operators.find(client.getNickname()) != operators.end() || regularUsers.find(client.getNickname()) != regularUsers.end())
        return true;
    return false;
}
bool Channel::isfull(){
    if (operators.size() + regularUsers.size() == limit)
        return true;
    return false;
}