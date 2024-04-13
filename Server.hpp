/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:16:27 by khanhayf          #+#    #+#             */
/*   Updated: 2024/04/13 16:39:13 by khanhayf         ###   ########.fr       */
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
#include "Client.hpp"
#include "Channel.hpp"
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <string>
#include <cstdlib>
#include <sstream>
#include <map>
#include <fstream>

#include "responses.hpp"

class Client;
class Channel;
//TODO : need canonical form for classes
class	Server{
	private:
		int									serverFD;
		int									port;
		static bool							signal;
		std::string							password;
		int									connectionID;
		std::string 						command;
		std::string 						args;
		std::vector<struct pollfd>			fds;
		// std::map<std::string, std::string>	map;
	public:
		std::vector<std::string>			sayingsBox;
		std::vector<Client>					clients;
		std::vector<Channel>				channels; 
		Server();
		~Server();//close users fds before quitting
		//--Setters--//
		void		setPort(int n);
		void		setPassword(char *str);
		//--Getters--//
		int			getPort();
		std::string	getPassword();
		int			getServerFD();
		//----//
		void		create_socket();
		void		launch_server();
		void		multi_clients();
		void		acceptClient();
		void		recieve_data(int fd);
		static void	sigHandler(int signum);
		void		closeFD();
		void		clearClient(int fd);

		void		addChannel(Channel const& channel);
		bool    	isInUseNickname(std::string nickname);
		bool    	isInUseChName(std::string chName);
		void		sendMsg(int clientFd, std::string msg);
		void		handleCommands(int i);
		bool		isRegistered(std::string nickname);
		// bool		isMember(Client &c, Channel &ch);
		Client		&findClient(std::string nn);
		Channel		&findChannel(std::string chname);

		void		clearClientslist();
		void		clearChannelslist();
		// void	channelsInvited2(Client & c);

		void	fillSayingsBox(std::string fileName);
};

void    tolowercase(std::string &str);
bool	isValidNickName(std::string nickname);
void    nickCommand(std::string &args, Client &c, Server &s);
void    userCommand(std::string &args, Client &c, Server &s);
void    passCommand(std::string &args, Client &c, Server &s);
void	inviteCommand(std::string &args, Client &c, Server &s);
void    modeCommand(std::string &args, Client &c, Server &s);
void    botCommand(Client &c, Server &s);

#endif