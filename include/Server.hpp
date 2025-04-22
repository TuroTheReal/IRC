/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:24:38 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/22 15:59:58 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <csignal>
#include <algorithm>
#include <unistd.h>
#include <vector>
#include <map>
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
		~Server(){std::cout << "Server closed" << std::endl;}

	private:
		int									_port;
		std::string							_password;
		int									_server_socket;
		std::string							_server_name;
		std::string							_host_name;
		std::vector<pollfd>					_fds;
		std::map<int, std::string>			_clientBuffers;
		std::map<std::string, std::string>	_errorCodes;

		void	createSocket();
		void	bindSocket();
		void	listenSocket();
		void	run();
		void	initErrorCodes();
		void	getHostName();
		void	newClient();
		void	handleClient(size_t index);
		void	removeClient(size_t index);
		void	cleanup();
		void	parseCommand(std::string, int client_fd);
		void	processClientBuffer(int client_fd);
		void	sendClientError(int client_fd, const std::string& key, const std::string& cmd);

		void	handleJoin(int, std::vector<std::string>);
		void	handleInvite(int, std::vector<std::string>);
		void	handlePrivmsg(int, std::vector<std::string>);
		void	handleKick(int, std::vector<std::string>);
		void	handlePass(int, std::vector<std::string>);
		void	handleNick(int, std::vector<std::string>);
		void	handleMode(int, std::vector<std::string>);
		void	handleCap(int, std::vector<std::string>);
		void	handleUser(int, std::vector<std::string>);
		void	handleTopic(int, std::vector<std::string>);
		void	handlePing(int, std::vector<std::string>);
		void	handleQuit(int, std::vector<std::string>);

		std::vector<Client*>		_clients;
		// std::vector<Channels*>	_channels;
};