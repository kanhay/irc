/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 23:26:29 by khanhayf          #+#    #+#             */
/*   Updated: 2024/04/04 13:04:07 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	inviteCommand(std::string &args, Client &c, Server &s){
    if (args.empty() || !args[0] || args[0] == ':' || !c.isRegistered()){
        if (!c.isRegistered())
            s.sendMsg(c.getClientFD(), "You have not registered.\n");
        else if (args.empty() || !args[0])
            s.sendMsg(c.getClientFD(), "End of Invite List.\n"); //no parameters
        else
            s.sendMsg(c.getClientFD(), "Not enough parameters.\n");
        return ;
    }
    std::istringstream iss(args);
    std::string guest;
    std::string chan;
    iss >> guest;
    std::cout << "username is " << guest << "\n";
    if (iss.fail() || iss.eof()){ //in case no channel given
        s.sendMsg(c.getClientFD(), "Not enough parameters.\n");
        return ;
    }
    if (s.isInUseNickname(guest)){
        std::cout << s.isInUseNickname(guest) << "\n";
        Client &clt = s.findClient(guest);
        if (!clt.isRegistered()){
            s.sendMsg(c.getClientFD(), "No such nick.\n");
            return ;}
        iss >> chan;
        if (chan[0] == '#')
            chan = chan.substr(1);
        else{
            s.sendMsg(c.getClientFD(), "No such channel.\n");
            return ;
        }
        if (!s.isInUseChName(chan)){
            s.sendMsg(c.getClientFD(), "No such channel.\n");
                return ;
        }
        else{
            Channel &ch = s.findChannel(chan);
            if (!ch.isOperator(clt)){
                s.sendMsg(c.getClientFD(), "You're not channel operator.\n");
                return ;
            }
        }
    }
    else{
        s.sendMsg(c.getClientFD(), "No such nick.\n");
        return ;
    }
}
