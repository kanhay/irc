/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 16:25:57 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/28 22:28:35 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>

#include "client.hpp"
#include "channel.hpp"

class Server{
        std::vector<Client> clients; //the currently connected clients in the server (the key in map is the client's nickname)
        std::vector<Channel> channels; //Available channels in the server
    public:
        Server();
        ~Server();//close users fds before quitting
        void    errorMessage(std::string msg);

        void    startServer();
        void    stopServer();
        void    handleConnections();
        void    broadcastMessage(std::string &msg, std::string chname);
        void    sendMessage(std::string &msg, std::string nickname); //to one specific user
        void    processCommand(std::string &cmd);

        //managing users
        bool    isInUseNickname(std::string nickname); //true if a nickname is already choosed by another client
    
        void    addUser(Client const& client); //add the new client in the clients container (only once)

        void    removeUser(Client const& client);
        

        //managing channels
        bool    isInUseChName(std::string chName); //true if there an other channel with the same name
        
        void    addChannel(Channel const& channel);

        void    removeChannel(Channel const& channel);

        

        //commands
};

#endif