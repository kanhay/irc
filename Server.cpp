/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:16:33 by khanhayf          #+#    #+#             */
/*   Updated: 2024/04/07 23:35:46 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

bool	Server::signal = false;

void	Server::sigHandler(int signum){
	(void)signum;
	std::cout << "signal found!" << std::endl;
	signal = true;//to stop the server
}
Server::Server(){
	serverFD = -1; 
	password = "\0";
}

Server::~Server(){
	clearClientslist();//do not forget to close clients fd
	clearChannelslist();
}

void	Server::setPort(int n){
	this->port = n;
}

void	Server::setPassword(char *str){
	this->password = str;
}
int	Server::getPort(){
	return(this->port);
}
std::string	Server::getPassword(){
	return (this->password);
}
int	Server::getServerFD(){
	return serverFD;
}
void	Server::clearClient(int fd){
	for (size_t i = 0; i < fds.size(); ++i){//remove client from fds vector
		if(fds[i].fd == fd){
			fds.erase(fds.begin() + i);
			break ;
		}
	}
	for (size_t i = 0; i < clients.size(); ++i){//remove client from Clients vector
		if(clients[i].getClientFD() == fd){
			clients.erase(clients.begin() + i);
			break ;
		}
	}
}void	Server::closeFD(){
	for (size_t i = 0; i < clients.size(); ++i){//close clients fd
		std::cout << "client disconnected" << std::endl;
		close(clients[i].getClientFD());}
	if (serverFD == -1){//close server socket
		std::cout << "server disconnected" << std::endl;
		close(serverFD);}
	channels.clear();
	clients.clear();
}

void		Server::create_socket(){
	struct sockaddr_in serveraddress;
	struct pollfd		pollf;		
	memset(&serveraddress, 0, sizeof(serveraddress));
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(this->port);
	serveraddress.sin_addr.s_addr = INADDR_ANY;//any local machine address

	serverFD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFD == -1)
		throw (std::runtime_error("Server failed to get created!"));
	// std::cout << "Socket created successfully" << std::endl;
	int	val = 1;
	if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)//SET OPTIONS OF SOCKET: SOL_SOCKET:the option is defined at socket level/ SO_REUSADDR : the option that allows to reuse local addresses which can be useful in scenarios where you want to quickly restart a server on the same address and port after it has been stopped.
		throw (std::runtime_error("the reuse of the address has failed!"));
	// std::cout << "Setsocketopt successfully" << std::endl;
	if (fcntl(serverFD, F_SETFL, O_NONBLOCK) == -1)//PERFORM OPERATIONS ON FD : F_SETFL: THE OPERATION IS TO SET FILE STATUS FLAGS/ O_NONBLOCK : SOCKET NONBLOCKING
		throw ("Failed to set nonblocking flag!");
	std::cout << "fcntl successfully" << std::endl;
	if (bind(serverFD, (const sockaddr *)&serveraddress, sizeof(serveraddress)) == -1)//bind the server to a port and IP
		throw(std::runtime_error("Binding to IP and port failed!"));
	// std::cout << "Socket binded" << std::endl;
	if (listen(serverFD, SOMAXCONN) == -1)//socket is passive and listening to coming connections
		throw (std::runtime_error("server isn't listening!"));
	std::cout << "Socket listening ..." << std::endl;
	pollf.fd = serverFD;//initialize the fds with server
	pollf.events = POLLIN;//flag to indicate theres data to read
	fds.push_back(pollf);//initialize fds vector
	std::cout << "server is listening from port : " << this->port << std::endl;
}
void	Server::launch_server(){
	create_socket();
	multi_clients();
}

void	Server::acceptClient(){
	Client				client;
	struct sockaddr_in	clientaddress;
	struct pollfd		newpool;
	socklen_t			clientaddrlen = 0;

	memset(&clientaddress, 0, sizeof(clientaddress));
	this->connectionID = accept(serverFD, (struct sockaddr *)&clientaddress, &clientaddrlen);//new socket to 	assure safe communication with multiple clients 
	if (connectionID == -1){
		std::cerr << "Failed to connect!" << std::endl;
		return ;
	}
	if (send(connectionID, "enter: password, nickname, and username\n", 41, 0) == -1)
		throw (std::runtime_error("failed to send to client"));
	if (fcntl(connectionID, F_SETFL, O_NONBLOCK) == -1){//server configuration can impact client's so we set new socket as server 
		std::cerr << "failed to set nonblocking option!" << std::endl;
		return ;
	}
	newpool.fd = connectionID;
	newpool.events = POLLIN;

	client.setClientFD(connectionID);
	client.setIP(inet_ntoa(clientaddress.sin_addr));
	clients.push_back(client);
	fds.push_back(newpool);
	std::cout << "accepted!" << std::endl;
}

void	to_lower(std::string &command){
	for (size_t i = 0; i < command.size(); ++i){
		command[i] = std::tolower(command[i]);
	}
}
std::string	skip_spaces(std::string str){
	for (size_t i = 0; i < str.size(); ++i){
		if (str[i] != ' ')
			return (&str[i]);
	}
	return (str);
}

void	Server::recieve_data(int fd){
	char	buffer[1024];

	memset(buffer, 0, sizeof(buffer));
	size_t	total = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (total <= 0){
		std::cout << "client gone" << std::endl;
		clearClient(fd);
		close(fd);
	}
	else{
		std::string	buf = buffer;
		 size_t fond;
		std::string	new_buf = skip_spaces(buf);
		for(size_t i = 0; i <= new_buf.size(); i++){
			fond = new_buf.find_first_of("\n");
			if (fond == std::string::npos)
				return;
			std::string	commond = new_buf.substr(0, fond);
			size_t	sp = commond.find_first_of("\t\r ");
			if (sp != std::string::npos){
				size_t	ind = sp;
				while (commond[ind] == '\t' || commond[ind] == '\r' || commond[ind] == ' ')
					ind++;
				if (commond[ind] == '\n')
					this->args = '\0';
				else
					this->args = commond.substr(ind, fond);
				this->command = commond.substr(0, sp);
			}
			else{
				this->command = commond.substr(0, fond); 
				this->args = '\0';
			}
			new_buf = new_buf.substr(fond+1, new_buf.size());
			handleCommands(fd);
			command.clear();
			args.clear();

		}
	}
}

void	Server::multi_clients(){
	while (Server::signal == false){
		if (poll(&fds[0], fds.size(), -1) == -1 && Server::signal == false)//poll blocked indefinitely till an event occurs or ctrl c
			throw (std::runtime_error("poll() failed!"));
		for (size_t i = 0; i < fds.size(); ++i){
			if (fds[i].revents & POLLIN)//returned event: data to read
			{
				if (fds[i].fd == serverFD)
					acceptClient();
				else
					recieve_data(fds[i].fd);
			}
		}
	}
	closeFD();
}

// void    Server::addClient(Client const& client){
//     this->clients.push_back(client);
// }

void	Server::sendMsg(int fd, std::string msg){
	if (send(fd, msg.c_str(), msg.size(), 0) == -1)
		throw (std::runtime_error("failed to send to client"));
}

bool    Server::isInUseNickname(std::string nickname){
    for (unsigned int i = 0; i < clients.size(); i++){
        if (clients[i].getNickname() == nickname)
            return true;
    }
    return false;
}

void	Server::handleCommands(int fd){
	tolowercase(command);
	unsigned int i = 0;
	for (i = 0; i < clients.size(); i++){
		if (clients[i].getClientFD() == fd){
			break ;
		}	
	}
	if (i == clients.size()) //this is not part of the implementation just in case this happens
		std::cout << "Client no found in container\n";
	if (command == "user")
		userCommand(args, this->clients[i], *this);
	else if (command == "nick")
		nickCommand(args, clients[i], *this);
	else if (command == "pass")
		passCommand(args, clients[i], *this);
	else if (command == "invite")
		inviteCommand(args, clients[i], *this);
	else if (command == "mode")
		modeCommand(args, clients[i], *this);
		
	// std::cout << "----------------------from the server -------------------------------------\n";
    //     std::cout << "------after cmd------\n";
    //     std::cout << "nn = " << clients[i].getNickname() << "\n";
    //     std::cout << "un = " << clients[i].getUsername() << "\n";
    //     std::cout << "hn = " << clients[i].getHostname() << "\n";
    //     std::cout << "sn = " << clients[i].getServername() << "\n";
    //     std::cout << "rn = " << clients[i].getRealname() << "\n";
    //     std::cout << "pw = " << clients[i].isPasswordSended() << "\n";
    //     std::cout << "registered = " << clients[i].isRegistered() << "\n";
    //     std::cout << "fd = " << clients[i].getClientFD() << "\n";
}

void    Server::addChannel(Channel const& channel){
    channels.push_back(channel);
}

bool	Server::isRegistered(std::string nickname){
	for (unsigned int i = 0; i < clients.size(); i++){
        if (clients[i].getNickname() == nickname && clients[i].isRegistered())
            return true;
    }
	return false;
}

bool    Server::isInUseChName(std::string chName){
    for (unsigned int i = 0; i < channels.size(); i++){
        if (channels[i].getName() == chName)
            return true;
    }
    return false;
}

// bool 	Server::isMember(Client &c, Channel &ch){
// 	for (unsigned int i = 0; i < channels.size(); i++){
//         if (channels[i].getName() == ch.getName() && ch.isMember(c))
//             return true;
//     }
//     return false;
// }

Client		&Server::findClient(std::string nn){
	unsigned int i;
	for (i = 0; i < clients.size(); i++){
		if (clients[i].getNickname() == nn)
			return (clients[i]);
	}
	return clients[i]; //clients end if not found
}

Channel		&Server::findChannel(std::string chname){
	unsigned int i;
	for (i = 0; i < channels.size(); i++){
		if (channels[i].getName() == chname)
			return (channels[i]);
	}
	return channels[i];//channels end if not found
}

// void	Server::channelsInvited2(Client & c){
// 	for (unsigned int i = 0; i < channels.size(); i++){
// 		if (channels[i].isInvited(c));
			
// 	}
// }

void	Server::clearClientslist(){
	channels.clear();
}
void	Server::clearChannelslist(){
	clients.clear();
}
