/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:24:38 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/16 14:46:50 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <cerrno>
#include "Client.hpp"

class Server {
	public:
		Server(int port, std::string password);
		~Server(){}

	private:
		int						_port;
		std::string				_password;
		int						_server_socket;
		std::vector<pollfd>		_fds;

		void createSocket();
		void bindSocket();
		void listenSocket();
		void run();
		void newClient(); //accept + push dans _fds
		void handleClient(size_t index); //buffer + recv pour recevoir les donnees
		void removeClient(size_t index); //close + clean data + (erase (begin + i)
		// void cleanup(); // shutdowm ou SIGINT/SIGTERM == fin boucle + cleanup ?
		std::vector<Client*>		_clients;
		// std::vector<Channels*>	_channels;
};