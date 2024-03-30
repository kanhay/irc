/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 13:41:32 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/30 12:47:36 by khanhayf         ###   ########.fr       */
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
        bool registered; //Indicates whether the client succeeded to register to the server or not
        bool PasswordSended; //set to true the first time the correct password is given by the client
        //set registred to 1 just before the client's autenticaation
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
    std::string getServername() const;
    std::string getRealname() const;
    int getSocketDescriptor() const;
    bool isRegistered() const; //return registred
    bool    isPasswordSended(); //return passwordsended

    //setters
    void setNickname(std::string nn);
    void setUsername(std::string un);
    void setHostname(std::string hn);
    void setServername(std::string sn);
    void setRealname(std::string rn);
    void setSocketDescriptor(int sd);
    void setRegistered(bool b);
    void    setPasswordSended(bool b);
    //others
    void joinchannel(Channel &channel);

    void user(std::string un, std::string hn, std::string sn, std::string rn);
    void registerClient(); //register the client into the server if all the attributes related to the client's autentication are set

};


#endif