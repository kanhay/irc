/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 16:26:14 by khanhayf          #+#    #+#             */
/*   Updated: 2024/03/31 18:34:16 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	Server::signal = false;

Server::Server(){
	serverFD = -1; 
	password = "\0";
	nick = "tikchbila";
	user = "tiwliwla";
}

Server::~Server(){
    for (unsigned int i = 0; i < clients.size(); i++)
        close(clients[i].getClientFD());//close users fd before quitting 
}

//setters

void	Server::setPort(int n){
	port = n;
}

void	Server::setPassword(char *str){
	password = str;
}

//getters

int	Server::getPort(){
	return(this->port);
}

std::string    Server::getPassword(){
    return password;
}

//building the server
void	Server::sigHandler(int signum){
	(void)signum;
	std::cout << "signal found!" << std::endl;
	signal = true;//to stop the server
}

void	Server::clear1Client(int fd){
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
	close(fd);
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
	if (send(connectionID, "enter: password, nickname, and username\n", 40, 0) == -1)
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
	std::cout << "lets see" << str << std::endl;
	return (str);
}

void	Server::recieve_data(int fd){
	char	buffer[1024];
	// Client	c;

	memset(buffer, 0, sizeof(buffer));
	size_t	total = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (total <= 0){
		std::cout << "client gone" << std::endl;
		clear1Client(fd);
	}
	else{
		std::string	buf = buffer;
		 size_t fond;
		std::string	new_buf = skip_spaces(buf);
		for(size_t i = 0; i <= new_buf.size(); i++){
			fond = new_buf.find_first_of("\t\r\n");
			if (fond == std::string::npos)
				return;
			std::cout << "content of fond++" << new_buf[fond] << "++" << std::endl;	
			std::string	commond = new_buf.substr(0, fond);
			std::cout << "command:" << commond << "--" << std::endl;
			size_t	sp = commond.find_first_of(" ");
			this->command = commond.substr(0, sp);
			std::cout << "com:" << this->command << "--" << std::endl;
			new_buf = new_buf.substr(fond+1, new_buf.size());
			this->args = skip_spaces(commond.substr(sp + 1, commond.length()));
			std::cout << "argu:" << this->args << "--" << std::endl;
			std::cout << "new_buff :" << &new_buf[i] << std::endl;
		}
	}
	// handleCommands();
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
	stopServer();
}

//managing users
bool    Server::isInUseNickname(std::string nickname){
    for (unsigned int i = 0; i < clients.size(); i++){
        if (clients[i].getNickname() == nickname)
            return true;
    }
    return false;
}

void    Server::addClient(Client const& client){
    clients.push_back(client);
}

// void    Server::removeClient(Client const& client){
//     for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++){
//         if (it->getNickname() == client.getNickname()){
// 			close(client.getClientFD()); //close client fd
//             clients.erase(it);
//             return ;
//         }
//     }
// }

void	Server::clearClients(){
	for (unsigned int i = 0; i < clients.size(); i++)
		close(clients[i].getClientFD());
	clients.clear();
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

void    Server::clearChannel(Channel const& channel){
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

// void	Server::handleCommands(Client &client){
// 	tolowercase(command);
// 	if (command == "user")
// 		userCommand(args, client);
// 	else if (command == "nick")
// 		nickCommand(args, client);
// 	else if (command == "pass")
// 		passCommand(args, client);
// }

void	Server::stopServer(){
	channels.clear();
	clearClients();
	close(serverFD);
}
