/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 16:25:57 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/27 14:09:20 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <map>
#include <vector>

#include "client.hpp"
#include "channel.hpp"

class Server{
        std::map<std::string, Client> clients; //the currently connected clients in the server (the key in map is the client's nickname)
        std::map<std::string, Channel> channels; //Available channels in the server
    public:
        Server();
        ~Server();
        void    errorMessage(std::string msg);

        bool    isInUseNickname(std::string nickname); //true if a nickname is already choosed by another client
        bool    isInUseUsername(std::string username); //true if a nickname is already setted by another client
    
        void    addClient(Client const& client); //add the new client in the clients container (only once)
        
        bool    isInUseChName(std::string chName); //true if there an other channel with the same name

        void    addChannel(Channel const& channel);
};

#endif