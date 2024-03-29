/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 13:41:32 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/28 22:33:55 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>

#include "channel.hpp"
class Client{
    private:
        std::string nickname; //the user's visible identity in IRC
        std::string username; //less visible identifier primarily used for server-side purposes
        std::string hostname; //the hostname of the client's machine
        std::string servername; //the IRC server to which the user is connecting
        std::string realname; //the real name of the client
        int socket; //The socket descriptor for the client's connection
        bool registred; //Indicates whether the client succeeded to register to the server or not
        int serverPort;
        
        Server &server;
        //containers
        // std::vector<std::string> joinedChan; //the list of channels joined by the client //names of channels //there is a member function ismember in channel class
        
    public:

    Client();
    ~Client(/*close sd*/);
    //getters
    std::string getNickname() const;
    std::string getUsername() const;
    std::string getHostname() const;
    std::string getRealname() const;
    int getSocketDescriptor() const;
    bool isRegistred() const; //return registred

    //setters
    void setNickname(std::string nn);
    void setUsername(std::string un);
    void sethostname(std::string hn);
    void setservername(std::string sn);
    void setrealname(std::string rn);
    void setSocketDescriptor(int sd);
    void setRegistered(bool b);
    //others
    void joinchannel(Channel &channel);

    void user(std::string un, std::string hn, std::string sn, std::string rn);
    

};


#endif