/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 13:33:00 by khanhayf          #+#    #+#             */
/*   Updated: 2024/04/06 20:42:55 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// void    xec_mode(std::string &modes, std::string &key, std::string &limit, std::string &user, Channel &ch){
//     char sign = '+';
//     for (unsigned int i = 0; i < modes.size(); i++){
//         if (modes[i] == '+' || modes[i] == '-')
//             sign = modes[i++];
//         if (modes[i] == 'k'){
//             ch.setHasKey(true);
//             ch.setKey(key);
//         }
//         else if (modes[i] == 'l'){
//             unsigned int nb;
//             std::stringstream ss(limit);
//             if(ss >> nb){
//                 ch.setLimit(nb);
//                 ch.setHasLimit(true);}
//         }
//         else if (modes[i] == 'i'){
//             ch.setMode("invite-only");
//         }
//         else if (modes[i] == 't'){
//             ch.setTopicLock(true);
//             //set the topic if there is a param after +t
//         }
//         else if (modes[i] == 'o'){
//             if (s.isInUseNickname(user) && s.isRegistered(user))
//                 ch.addOperator(s.findClient(user)); //check if the nickname is in use and if its registered then add it in addoprator
//         }
//     }
// }

void    modeCommand(std::string &args, Client &c, Server &s){
    if (args.empty() || !args[0] || args[0] != '#'){
        if (args.empty() || !args[0])
            s.sendMsg(c.getClientFD(), "Not enough parameters.\n");
        else
            s.sendMsg(c.getClientFD(), "No such channel.\n");
        return ;
    }
    std::istringstream iss(args);
    std::string chan;
    args = args.substr(1);
    iss >> chan;
    if (!s.isInUseChName(chan)){
        s.sendMsg(c.getClientFD(), "No such channel.\n");
        return ;
    }
    Channel &channel = s.findChannel(chan);
    if (!channel.isOperator(c)){
        s.sendMsg(c.getClientFD(), "You are not an chanop.\n");
        return ;
    }
    std::string modestring, key, limit, user, list;
    int k, o, l;
    k = o = l = 0;
    std::string modes = "+-litok";
    while (iss >> modestring){
    for (unsigned i = 0; i < modestring.size(); i++){
        if (modes.find(modestring[i]) == std::string::npos){
            s.sendMsg(c.getClientFD(), "There is unknown mode char to me.\n");
            return ;
        }
        else{
            while (modestring[i] == modestring[i + 1])//ADD the last element when repeated
                i++;
            if (modestring[i] == 'k' && k++ == 0) //read from iss only the first time k is found
                iss >> key;
            if (modestring[i] == 'l'  && l++ == 0)
                iss >> limit;
            if (modestring[i] == 'o' && o++ == 0)
                iss >> user;
            }
            list += modestring[i];
        }
    }
    // xec_mode(list, key, limit, user, channel);
    //xec
    char sign = '+';
    for (unsigned int i = 0; i < list.size(); i++){
        if (list[i] == '+' || list[i] == '-')
            sign = list[i++];
        if (list[i] == 'k'){
            if (!key.empty()){
                if (sign == '+'){
                    if (!channel.hasAKey()){
                        channel.setHasKey(true);
                        channel.setKey(key);}
                    else
                        s.sendMsg(c.getClientFD(), "Key already set.\n");
                }
                else{
                    channel.setHasKey(false);
                    channel.setKey("");
                }
            }
            else
                s.sendMsg(c.getClientFD(), "Not enough parameters.\n");
        }
        else if (list[i] == 'l'){
            if (!limit.empty()){
                unsigned int nb;
                std::stringstream ss(limit);
                if (ss >> nb){
                    if (sign == '+'){
                        channel.setLimit(nb);
                        channel.setHasLimit(true);
                    }
                    else{
                        channel.setLimit(0);
                        channel.setHasLimit(false);}
                }
            }
            else
                s.sendMsg(c.getClientFD(), "Not enough parameters.\n");
        }
        else if (list[i] == 'i'){
            if (sign == '+')
                channel.setMode("invite-only");
            else
                channel.setMode("");
            
        }
        else if (list[i] == 't'){
            if (channel.isOperator(c)){
                if (sign == '+')
                    channel.setTopicLock(true);
                else
                    channel.setTopicLock(false);
            }
        }
        else if (list[i] == 'o'){
            if (!user.empty()){
                if (!s.isInUseNickname(user))
                    s.sendMsg(c.getClientFD(), "No such nick.\n");
                if (channel.isMember(s.findClient(user))){
                    if (sign == '+')
                        channel.addOperator(s.findClient(user)); //check if the nickname is in use and if its registered then add it in addoprator
                    else
                        channel.addRegularUser(s.findClient(user)); //check if the nickname is in use and if its registered then add it in addoprator
                }
                else
                    s.sendMsg(c.getClientFD(), "Not member in the channel.\n");
            }
            else
                s.sendMsg(c.getClientFD(), "Not enough parameters.\n");
        }
    }
}


    //         if (modestring[i] == 'k' && modestring[i + 1] == ' '){
    //             i += 2;
    //             key = modestring.substr(i, modestring.find_first_of(" "));
    //         }
    //         if (modestring[i] == 'l' && modestring[i + 1] == ' '){
    //             i += 2;
    //             limit = modestring.substr(i, modestring.find_first_of(" "));
    //         }
    //         list += list[i];
    //     }
    // }
    // if (list.find('k') != std::string::npos)
    //     iss >> pw;
    // if (list.find('l') != std::string::npos)
    //     iss >> limit
    
// }
//MODE #channel +t  //(topic settable by channel operator only)
//MODE #channel +o nickname   //gives operator status to the user with the nickname "nickname" in the channel "#channel". 