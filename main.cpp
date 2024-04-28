/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khanhayf <khanhayf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:17:48 by khanhayf          #+#    #+#             */
/*   Updated: 2024/04/27 17:44:39 by khanhayf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

int	main(int ac, char **av){
	Server	serv;
	try{
		int			por;
		std::string	pas;
	
		if (ac != 3){
			std::cerr << "Enter a port and a password" << std::endl;
			return (1);
		}
		serv.setPort(strtol(av[1], NULL, 10));
		std::string password = av[2];
		if (password.find_first_of(" \t") != std::string::npos)
			throw(std::runtime_error("White spaces are not accepted."));
		serv.setPassword(av[2]);
		por = serv.getPort();
		std::string	s = av[1];
		if (s.find_first_not_of("0123456789") != std::string::npos){
			std::cerr << "Only numbers in port!" << std::endl;
			return (1);
		}
		pas = serv.getPassword();
		if (por < 1024 || por > 49151){
			std::cerr << "Enter a valid port" << std::endl;
			return (1);
		}
		if (pas.size() <= 0){
			std::cout << "Empty Password!" << std::endl;
			return (1);
		}
		signal(SIGINT, Server::sigHandler);
		signal(SIGQUIT, Server::sigHandler);
		serv.launch_server();
	}
	catch(const std::exception &e){
		serv.closeFD();
		std::cout << e.what() << std::endl;
	}
}
