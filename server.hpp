/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 16:25:57 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/31 18:34:18 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <string>

#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server{
    private:
		int							serverFD;
		int							port;
		int							connectionID;
		static bool					signal;
		std::string					password;
		std::string					nick;
		std::string					user;
		std::string 				command;
		std::string 				args;
		std::vector<struct pollfd>	fds;

        std::vector<Client> clients; //the currently connected clients in the server
        std::vector<Channel> channels; //Available channels in the server
    public:
        Server();
        ~Server();//close users fds before quitting

        //setters
        void		setPort(int n);
		void		setPassword(char *str);
        
        //getters
		int				getPort();
        std::string		getPassword();
        
        //building the server
		void		create_socket();
		void		launch_server();
		void		multi_clients();
		void		acceptClient();
		void		recieve_data(int fd);
		static void	sigHandler(int signum);

		void		sendMsg(int clientFd, std::string msg); //needed to send error messages to a specific client
		void		stopServer();

        //managing users
        bool    isInUseNickname(std::string nickname); //true if a nickname is already choosed by another client
        void    addClient(Client const& client); //add the new client in the clients container (only once)
		void	clear1Client(int fd);
        // void    removeClient(Client const& client); //close fd when removing user
		void	clearClients(); //close all clients fd and clear clients

        //managing channels
        bool    isInUseChName(std::string chName); //true if there an other channel with the same name
        void    addChannel(Channel const& channel);
        void    clearChannel(Channel const& channel);
		// other
		// void	handleCommands(int fd);

};

#endif