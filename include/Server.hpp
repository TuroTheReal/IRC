/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:24:38 by dsindres          #+#    #+#             */
/*   Updated: 2025/05/21 13:36:36 by dsindres         ###   ########.fr       */
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
#include "Channel.hpp"
#include "Command.hpp"

class Server {
	public:
		Server(int port, std::string password);
		~Server();

	private:

		int										_port;
		std::string								_password;
		int										_server_socket;
		std::string								_server_name;
		std::string								_host_name;
		std::vector<pollfd>						_fds;
		std::map<int, std::string>				_clientBuffers;
		std::map<std::string, std::string>		_errorCodes;
		std::vector<Client*>					_clients;
		std::vector<Channel*>					_channels;

		void	createSocket();
		void	bindSocket();
		void	listenSocket();
		void	run();

		void	initErrorCodes();
		void	getHostName();
		void	sendClientError(int, const std::string&, const std::string&);
		void	sendWelcome(int, Client*);

		void	newClient();
		void	handleClient(size_t);
		void	removeClient(size_t);
		void	removeClientByFD(int);
		void	cleanup();

		void	parseCommand(std::string, int);
		void	processClientBuffer(int);

		void	handleCommandBot(int, std::vector<std::string>, Client*);
		void	handleCommandBotPriv(int, std::vector<std::string>, Client*);
		void	handleJoin(int, std::vector<std::string>, Client*);
		void	handleInvite(int, std::vector<std::string>, Client*);
		void	handlePrivmsg(int, std::vector<std::string>, Client*);
		void	handleKick(int, std::vector<std::string>, Client*);
		void	handlePass(int, std::vector<std::string>, Client*);
		void	handleNick(int, std::vector<std::string>, Client*);
		void	handleMode(int, std::vector<std::string>, Client*);
		void	handleCap(int, std::vector<std::string>, Client*);
		void	handleUser(int, std::vector<std::string>, Client*);
		void	handleTopic(int, std::vector<std::string>, Client*);
		void	handlePing(int, std::vector<std::string>);
		void	handleQuit(int);

		Client*	getClientByFD(int);
		bool	isValidChannel(std::string);
		bool	isValidNickname(std::string);
		bool	isValidUsername(std::string);
		bool	isNotTakenChannel(std::string);


		// debug
		void	handle2X(int, std::vector<std::string>, Client*);
		void	handle3X(int, std::vector<std::string>, Client*);
};