/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 23:26:29 by khanhayf          #+#    #+#             */
/*   Updated: 2024/04/07 18:27:17 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	inviteCommand(std::string &args, Client &c, Server &s){
        if (!c.isRegistered()){
            s.sendMsg(c.getClientFD(), ERR_NOTREGISTERED(c.getNickname()));
            return ;
        }
    std::stringstream ss(args);
    std::string guest;
    std::string chan;
    if (args[0] == ':'){
        args = args.substr(1);
        ss.clear();
        ss << args;
        getline(ss, guest);}
    else
        ss >> guest;
    ss >> chan;
    if (guest.empty() || (guest == c.getNickname() && chan.empty())){
        s.sendMsg(c.getClientFD(), RPL_ENDOFINVITE(c.getNickname()));//End of Invite List
        return ;
    }
    if (s.isInUseNickname(guest)){
        Client &clt = s.findClient(guest);
        if (!clt.isRegistered()){
            s.sendMsg(c.getClientFD(), ERR_NOSUCHNICK(c.getNickname(), guest));
            return ;}
        if (chan[0] == '#')
            chan = chan.substr(1);
        else{
            s.sendMsg(c.getClientFD(), ERR_NOSUCHCHANNEL(chan, c.getNickname()));
            return ;
        }
        if (!s.isInUseChName(chan)){
            s.sendMsg(c.getClientFD(), ERR_NOSUCHCHANNEL(("#" + chan), c.getNickname()));
                return ;
        }
        else{
            Channel &ch = s.findChannel(chan);//If a channel is set to invite-only (mode +i), a channel operator can use the INVITE command to invite a client to join the channel.
            if (!ch.isMember(c)){
                s.sendMsg(c.getClientFD(), ERR_NOTONCHANNEL(c.getNickname(), ("#" + chan)));
                return ;
            }
            if (ch.isMember(clt)){
                s.sendMsg(c.getClientFD(), ERR_USERONCHANNEL(("#" + chan), guest, c.getNickname()));
                return ;
            }
            if (!ch.isOperator(c) && ch.getMode() == "invite-only"){
                s.sendMsg(c.getClientFD(), ERR_NOTOP(c.getNickname(), ("#" + chan)));
                return ;
            }
            // ch.addInvited(clt);
            s.sendMsg(clt.getClientFD(), RPL_INVITE(c.getNickname(), c.getUsername(), clt.getHostname(), guest, ("#" + chan)));
        }
    }
    else{
        s.sendMsg(c.getClientFD(), ERR_NOSUCHNICK(c.getNickname(), guest));
        return ;
    }
}
