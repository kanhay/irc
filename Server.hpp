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
		int									serverFD; //M (name changed from serverID to severFD)
		int									port;
		static bool							signal;
		std::string							password;
		int									connectionID;
		std::string 						command;
		std::string 						args;
		std::vector<struct pollfd>			fds;
		// std::map<std::string, std::string>	map;
		std::vector<std::string>			sayingsBox;//M
		std::vector<Client>					clients;
		std::vector<Channel>				channels; 
	public:
		Server();
		~Server();//close users fds before quitting//M
		//--Setters--//
		void		setPort(int n);
		void		setPassword(char *str);
		//--Getters--//
		int			getPort();
		std::string	getPassword();
		int			getServerFD();//M
		//----//
		void		create_socket();
		void		launch_server();
		void		multi_clients();
		void		acceptClient();
		void		recieve_data(int fd);
		static void	sigHandler(int signum);
		void		closeFD();
		void		clearClient(int fd);

		void		addChannel(Channel const& channel);//M
		bool    	isInUseNickname(std::string nickname);//M
		bool    	isInUseChName(std::string chName);//M
		void		sendMsg(int clientFd, std::string msg);//M
		void		handleCommands(int i);//M
		bool		isRegistered(std::string nickname);//M
		Client		&findClient(std::string nn);//M
		Channel		&findChannel(std::string chname);//M

		void		clearClientslist();//M
		void		clearChannelslist();//M

		void	fillSayingsBox(std::string fileName);//M
        void    nickCommand(std::string &args, Client &c);//M
        void    userCommand(std::string &args, Client &c);//M
        void    passCommand(std::string &args, Client &c);//M
        void	inviteCommand(std::string &args, Client &c);//M
        void    modeCommand(std::string &args, Client &c);//M
        void    botCommand(Client &c);//M
};

void    tolowercase(std::string &str);//M
bool	isValidNickName(std::string nickname);//M

#endif