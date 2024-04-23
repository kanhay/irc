/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 23:26:29 by khanhayf          #+#    #+#             */
/*   Updated: 2024/04/22 11:33:46 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


void	Server::inviteCommand(std::string &args, Client &c){
// invite #ggg hh
// :*.freenode.net 403 uu hh :No such channel
// invite #ggg
// :*.freenode.net 337 uu :End of INVITE list
// invite
// :*.freenode.net 337 uu :End of INVITE list
// invite jj
// :*.freenode.net 337 uu :End of INVITE list
// invite tt #ff
// :*.freenode.net 442 tt #ff :You're not on that channel!

    // if (!c.isRegistered()){
    //     sendMsg(c.getClientFD(), ERR_NOTREGISTERED(c.getNickname()));
    //     return ;
    // }
    
    std::stringstream ss;
    std::string guest;
    std::string chan;
    if (args[0] == ':' && args[1] != '#'){
        args = args.substr(1);
        ss << args;
        getline(ss, guest);}
    else if ((args[0] != ':' && args[1] != '#') && args[0] != '#'){ //if the user gives the channel only without nickname
        ss << args;
        ss >> guest;}
    if (ss.eof() || guest.empty()){
        sendMsg(c.getClientFD(), RPL_ENDOFINVITE(c.getNickname()));//End of Invite List
        return ;
    }
    else{
        if (ss.peek() == ':')
            getline(ss, chan);
        else
            ss >> chan;
    }
    if (chan[0] == ':' && chan[1] == '#')
        chan = chan.substr(1);
    else if (chan[0] == '#'){
            sendMsg(c.getClientFD(), ERR_NOSUCHCHANNEL(chan, c.getNickname()));
            return ;
    }
    if (isInUseNickname(guest)){
        Client &clt = findClient(guest);
        if (!clt.isRegistered()){
            sendMsg(c.getClientFD(), ERR_NOSUCHNICK(c.getNickname(), guest));
            return ;}
        if (!isInUseChName(chan)){
            sendMsg(c.getClientFD(), ERR_NOSUCHCHANNEL(chan, c.getNickname()));
                return ;
        }
        Channel &ch = findChannel(chan);//If a channel is set to invite-only (mode +i), a channel operator can use the INVITE command to invite a client to join the channel.
        if (!ch.isMember(c)){
            sendMsg(c.getClientFD(), ERR_NOTONCHANNEL(c.getNickname(), chan));
            return ;
        }
        if (ch.isMember(clt)){
            sendMsg(c.getClientFD(), ERR_USERONCHANNEL(chan, guest, c.getNickname()));
            return ;
        }
        if (!ch.isOperator(c)){
            sendMsg(c.getClientFD(), ERR_NOTOP(c.getNickname(), chan, ""));//
            return ;
        }
        sendMsg(c.getClientFD(), RPL_INVITING(c.getNickname(), guest, ch.getName())); 
        sendMsg(clt.getClientFD(), RPL_INVITE(c.getNickname(), c.getUsername(), clt.getClientIP(), guest, ch.getName()));
        clt.invite2channel(chan);
    }
    else{
        sendMsg(c.getClientFD(), ERR_NOSUCHNICK(c.getNickname(), guest));
        return ;
    }
}