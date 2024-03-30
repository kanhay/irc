/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 16:26:14 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/30 21:37:38 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	Server::signal = false;

Server::Server(){
	serverID = -1; 
	password = "\0";
	nick = "tikchbila";
	user = "tiwliwla";
}

Server::~Server(){
    for (unsigned int i = 0; i < clients.size(); i++)
        close(clients[i].getSocketDescriptor());//close users fd before quitting 
}

//setters

void	Server::setPort(int n){
	port = n;
}

void	Server::setPassword(char *str){
	password = str;
}

//getters

std::string    Server::getPassword(){
    return password;
}


//building the server
void	Server::sigHandler(int signum){
	(void)signum;
	std::cout << "signal found!" << std::endl;
	signal = true;//to stop the server
}

void	Server::clearClient(int fd){
	for (size_t i = 0; i < fds.size(); ++i){//remove client from fds vector
		if(fds[i].fd == fd){
			fds.erase(fds.begin() + i);
			break ;
		}
	}
	for (size_t i = 0; i < clients.size(); ++i){//remove client from Clients vector
		if(clients[i].getClientID() == fd){
			clients.erase(clients.begin() + i);
			break ;
		}
	}
}

void	Server::closeFD(){
	for (size_t i = 0; i < clients.size(); ++i){//close clients fd
		std::cout << "client disconnected" << std::endl;
		close(clients[i].getClientID());}
	if (serverID == -1){//close server socket
		std::cout << "server disconnected" << std::endl;
		close(serverID);}
}

void		Server::create_socket(){
	struct sockaddr_in serveraddress;
	struct pollfd		pollf;		
	memset(&serveraddress, 0, sizeof(serveraddress));
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(this->port);
	serveraddress.sin_addr.s_addr = INADDR_ANY;//any local machine address

	serverID = socket(AF_INET, SOCK_STREAM, 0);
	if (serverID == -1)
		throw (std::runtime_error("Server failed to get created!"));
	// std::cout << "Socket created successfully" << std::endl;
	int	val = 1;
	if (setsockopt(serverID, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)//SET OPTIONS OF SOCKET: SOL_SOCKET:the option is defined at socket level/ SO_REUSADDR : the option that allows to reuse local addresses which can be useful in scenarios where you want to quickly restart a server on the same address and port after it has been stopped.
		throw (std::runtime_error("the reuse of the address has failed!"));
	// std::cout << "Setsocketopt successfully" << std::endl;
	if (fcntl(serverID, F_SETFL, O_NONBLOCK) == -1)//PERFORM OPERATIONS ON FD : F_SETFL: THE OPERATION IS TO SET FILE STATUS FLAGS/ O_NONBLOCK : SOCKET NONBLOCKING
		throw ("Failed to set nonblocking flag!");
	std::cout << "fcntl successfully" << std::endl;
	if (bind(serverID, (const sockaddr *)&serveraddress, sizeof(serveraddress)) == -1)//bind the server to a port and IP
		throw(std::runtime_error("Binding to IP and port failed!"));
	// std::cout << "Socket binded" << std::endl;
	if (listen(serverID, SOMAXCONN) == -1)//socket is passive and listening to coming connections
		throw (std::runtime_error("server isn't listening!"));
	std::cout << "Socket listening ..." << std::endl;
	pollf.fd = serverID;//initialize the fds with server
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
	this->connectionID = accept(serverID, (struct sockaddr *)&clientaddress, &clientaddrlen);//new socket to 	assure safe communication with multiple clients 
	if (connectionID == -1){
		std::cerr << "Failed to connect!" << std::endl;
		return ;
	}
	if (send(connectionID, "enter: password, nickname, and username\n", 40, 0) == -1)
		throw (std::runtime_error("failed to send to client"));
	if (fcntl(connectionID, F_SETFL, O_NONBLOCK) == -1){//server configuration can impact client's so we set new socket as server 
		std::cerr << "failed to set nonblocking option!" << std::endl;
		return ;
	}
	newpool.fd = connectionID;
	newpool.events = POLLIN;

	client.setClientID(connectionID);
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
	std::cout << "lets see" << str << std::endl;
	return (str);
}

void	Server::recieve_data(int fd){
	char	buffer[1024];
	Client	c;

	memset(buffer, 0, sizeof(buffer));
	size_t	total = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (total <= 0){
		std::cout << "client gone" << std::endl;
		clearClient(fd);
		close(fd);
	}
	else{
		std::string	buf = buffer;size_t fond;
		std::string	new_buf = skip_spaces(buf);
			fond = new_buf.find_first_of(" ");
			if (fond != std::string::npos){
				std::cout << "found space in here : " << fond << std::endl;
				this->command = new_buf.substr(0, fond);
				this->args = new_buf.substr(fond + 1, new_buf.length());
				std::cout << "command: " << this->command << std::endl;
				std::cout << "argu: " << this->args << std::endl;
			}
		// this->password += "\n";
		// if (strcmp(buffer, this->password.c_str())){
		// 	if (send(this->connectionID, "password :", 10, 0) == -1)
		// 		throw (std::runtime_error("failed to send to client"));
		// 	size_t	total = recv(fd, buffer, sizeof(buffer) - 1, 0);
		// 	if (total <= 0){
		// 		std::cout << "client gone" << std::endl;
		// 		clearClient(fd);
		// 		close(fd);
		// 	}
		// }
		// if (send(this->connectionID, "nickname :", 10, 0) == -1)
		// 		throw (std::runtime_error("failed to send to client"));
		// this->nick = buffer;
		// if (send(this->connectionID, "username :", 10, 0) == -1)
		// 		throw (std::runtime_error("failed to send to client"));
		// this->user = buffer;
		// std::cout << "user = " << this->nick << std::endl;
		// std::string	buf = buffer;
		// size_t found = buf.find(' ');
		// this->command = buf.substr(0, found);
		// this->args = buf.substr(found + 1, buf.length());
		// to_lower(this->command);
		// if (this->command == "join\n")
		// 	std::cout <<"join channel" << std::endl;
		// else if (this->command == "invite\n")
		// 	std::cout <<"invite user" << std::endl;
		// else if (this->command == "kick\n")
		// 	std::cout <<"kick user" << std::endl;
		// std::cout << "command : " << this->command << " args :" << this->args << std::endl;

	}
		// this->Clients[fd].setBuffer(buffer);
}

void	Server::multi_clients(){
	while (Server::signal == false){
		if (poll(&fds[0], fds.size(), -1) == -1 && Server::signal == false)//poll blocked indefinitely till an event occurs or ctrl c
			throw (std::runtime_error("poll() failed!"));
		for (size_t i = 0; i < fds.size(); ++i){
			if (fds[i].revents & POLLIN)//returned event: data to read
			{
				if (fds[i].fd == serverID)
					acceptClient();
				else
					recieve_data(fds[i].fd);
			}
		}
	}
	closeFD();
}

//managing users
bool    Server::isInUseNickname(std::string nickname){
    for (unsigned int i = 0; i < clients.size(); i++){
        if (clients[i].getNickname() == nickname)
            return true;
    }
    return false;
}

void    Server::addUser(Client const& client){
    clients.push_back(client);
}

void    Server::removeUser(Client const& client){
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++){
        if (it->getNickname() == client.getNickname()){
			close(client.getSocketDescriptor()); //close client fd
            clients.erase(it);
            return ;
        }
    }
}

bool    Server::isInUseChName(std::string chName){
    for (unsigned int i = 0; i < channels.size(); i++){
        if (channels[i].getName() == chName)
            return true;
    }
    return false;
}

void    Server::addChannel(Channel const& channel){
    channels.push_back(channel);
}

void    Server::removeChannel(Channel const& channel){
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++){
        if (it->getName() == channel.getName()){
            channels.erase(it);
            return ;
        }
    }
}

//other

void	Server::sendMsg(int clientFd, std::string msg){
	std::cerr << clientFd << ">> Error: " << msg <<"\n";
}

void	Server::handleCommands(std::string &cmd, std::string &args, Client &client){
	tolowercase(cmd);
	if (cmd == "user")
		userCommand(args, client);
	else if (cmd == "nick")
		nickCommand(args, client);
	else if (cmd == "pass")
		passCommand(args, client);
}

