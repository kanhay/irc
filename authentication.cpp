/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   authentication.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 15:39:46 by khanhayf          #+#    #+#             */
/*   Updated: 2024/04/03 19:45:51 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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
        return(false);//empty nickname
    return true;
}

void    nickCommand(std::string &args, Client &c, Server &s){
    if (args.empty()){
        s.sendMsg(c.getClientFD(), "No nickname given.\n");
        return ;
    }
    tolowercase(args); //Nicknames are generally case-insensitive
    std::string param;
    if (args[0] == ':')
        param = args.substr(1); //starting from index 1
    else
        param = args.substr(0, args.find_first_of(' '));
    if (!isValidNickName(param)){
        s.sendMsg(c.getClientFD(), "Erroneous Nickname\n");
        return;}
    if (s.isInUseNickname(param)){// check this only if new client
        if (c.getNickname() == param) //in case a client try to change his nn with the same nickname setted before
            return;
    s.sendMsg(c.getClientFD(), "Nickname is in use.\n");
    return ;}
    c.setNickname(param); //in both case: choosing a nname for the first time or changing a nname 
    if (!c.isRegistered()) //he can change the nick name after registration 
    c.registerClient(s);//client become registred in the server if the condition inside registerClient is true
}

void    userCommand(std::string &args, Client &c, Server &s){
    if (c.isRegistered()){
        s.sendMsg(c.getClientFD(), "You may not reregister.\n");
        return;}
    if (args[0] == ':'){//it's not standard practice to use colons before every parameter in IRC commands.//the colon is reserved specifically for the trailing(last) parameter.
        s.sendMsg(c.getClientFD(), "Invalid syntax for user command.\n");
        return;
    }
    std::istringstream iss(args);
    std::string un, hn, sn, rn;
    iss >> un >> hn >> sn;
    if (hn[0] == ':' || sn[0] == ':'){
        s.sendMsg(c.getClientFD(), "Invalid syntax for user command.\n");
        return ;
    }
    iss >> std::ws;
    if (iss.peek() == ':'){
        getline(iss, rn);
        rn = rn.substr(rn.find_first_not_of(":"));
    }
    else
        iss >> rn;
    // if (!iss.eof()){
    //     s.sendMsg(c.getClientFD(), "Invalid syntax for user command.\n");
    //     return ;
    // }
    if (!iss.fail() && !un.empty() && !hn.empty() && !sn.empty() && !rn.empty()){
        c.setUsername(un);
        c.setHostname(hn);
        c.setServername(sn);
        c.setRealname(rn);
        c.registerClient(s);//client become registred in the server if the condition inside registerClient is true
    }
    else
        s.sendMsg(c.getClientFD(), "Not enough parameters.\n");
}

void    passCommand(std::string &args, Client &c, Server &s){
    c.clearAuthentication();
    // std::cout << "args||" << args << "||\n";
    if (c.isRegistered()){
        s.sendMsg(c.getClientFD(), "You may not reregister.\n");
        return ;}
    if (!args.empty() && args[0]){
        std::string param;
        if (args[0] == ':'){
            // args = args.substr(1);
            // param = args;
            param = args.substr(1);
        }
        else
            param = args.substr(0, args.find_first_of(' '));
        // if (param != args){ //there is more argument in args 
        //     s.sendMsg(c.getClientFD(), "Invalid syntax for pass command.\n"); //optionnal
        //     return ;}
        if (param != s.getPassword()){
            // c.setPasswordSended(false);
            s.sendMsg(c.getClientFD(), "Incorrect password.\n");
            return;
        }
        else{
            c.setPasswordSended(true);  //leave a mark if pass cmd succeed the first time
            c.registerClient(s);//client become registred in the server if the condition inside registerClient is true
        }
    }
    else
        s.sendMsg(c.getClientFD(), "Not enough parameters.\n");
}
