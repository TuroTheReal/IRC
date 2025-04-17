/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:35:44 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/17 14:14:24 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Server::createSocket(){
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw std::runtime_error(std::string("Error: socket: ") + std::strerror(errno));
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(fd);
		throw std::runtime_error(std::string("Error: fcntl: ") + std::strerror(errno));
	}
	_server_socket = fd;
}

void Server::bindSocket(){
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);

	if (bind(_server_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	throw std::runtime_error(std::string("Error: bind: ") + std::strerror(errno));
}

void Server::listenSocket(){
	if (listen(_server_socket, 5) < 0) {
		throw std::runtime_error(std::string("Error: listen: ") + std::strerror(errno));
	}
}

volatile sig_atomic_t _isON = 1;

void handleSignal(int signum){
	(void)signum;
	_isON = 0;
}

void  Server::initErrorCodes(){
	_errorCodes["401"] = "No such nick/channel";
	_errorCodes["403"] = "No such channel";
	_errorCodes["421"] = "Unknown command";
	_errorCodes["431"] = "No nickname given";
	_errorCodes["432"] = "Erroneous nickname";
	_errorCodes["433"] = "Nickname is already in use";
	_errorCodes["441"] = "They aren't on that channel";
	_errorCodes["442"] = "You're not on that channel";
	_errorCodes["461"] = "Not enough parameters";
	_errorCodes["472"] = "Unknown mode char";
	_errorCodes["473"] = "Cannot join channel (+i)";
	_errorCodes["474"] = "Cannot join channel (+b)";
	_errorCodes["475"] = "Cannot join channel (+k)";
	_errorCodes["476"] = "Bad channel mask";
	_errorCodes["481"] = "Permission Denied (You're not an IRC operator)";
	_errorCodes["482"] = "You're not channel operator";
	_errorCodes["484"] = "Your connection is restricted";
	_errorCodes["501"] = "Unknown MODE flag";
	_errorCodes["502"] = "Cannot change mode for other users";
}

void Server::sendClientError(int client_fd, const std::string& key, const std::string& cmd){
	// std::vector<Client *>::iterator it = _clients.begin();
	std::string nickname = "*";

	// for (; it != _clients.end(); it++)
	// {
	// 	if (client_fd == (*it)->get_socket())
	// 		nickname = (*it)->get_nickname();
	// 	else
	// 		nickname = "*";
	// }

	std::string errorMsg;
	std::map<std::string, std::string>::iterator ite = _errorCodes.find(key);
	if (ite != _errorCodes.end())
		errorMsg = ite->second;
	else
		errorMsg = "Error";

	std::string to_send = ":" + _server_name + " " + key + " " + nickname;

	if (!cmd.empty())
		to_send += " " + cmd;

	to_send += errorMsg + "\r\n";

	ssize_t sent = send(client_fd, to_send.c_str(), to_send.length(), 0);
	if (sent < 0)
		throw std::runtime_error(std::string("Error: send: ") + std::strerror(errno));
}

void Server::newClient(){
	std::cout << "Connection received" << std::endl;

	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	int client_fd = accept(_server_socket, (struct sockaddr*)&client_addr, &addr_len);
	if (client_fd < 0){
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw std::runtime_error(std::string("Error: accept: ") + std::strerror(errno));
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(client_fd);
			throw std::runtime_error(std::string("Error: fcntl: ") + std::strerror(errno));
	}

	struct pollfd client;
	client.fd = client_fd;
	client.events = POLLIN;
	client.revents = 0;

	_fds.push_back(client);

	// Client* new_client = new Client(client_fd);
	// _clients.push_back(new_client);
}

void Server::removeClient(size_t index){
	// int fd = _fds[index].fd;
	close(_fds[index].fd);
	_fds.erase(_fds.begin() + index);

	// std::vector<Client*>::iterator it = _clients.begin();
	// std::vector<Client*>::iterator ite = _clients.end();

	// for (; it != ite; it++)
	// {
	// 	if ((*it)->get_socket() == fd)
	// 	{
	// 		delete *it;
	// 		_clients.erase(it);
	// 		break;
	// 	}
	// }
	// clean data dans les differentes classes : _client.erase(_fds[index].fd) || _client.erase(_fds[index])
	std::cout << "Client disconnected" << std::endl;
}

void Server::handleClient(size_t index){
	int client_fd = index;
	char buffer[1024];

	ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read == 0)
			std::cout << "Client Disconnected" << std::endl;
		else
			throw std::runtime_error(std::string("Error: recv: ") + std::strerror(errno));
		removeClient(index);
		return ;
	}
	buffer[bytes_read] = '\0';
	_clientBuffers[client_fd] += buffer;
	processClientBuffer(client_fd);
}

void Server::processClientBuffer(int client_fd){
	std::string& buffer = _clientBuffers[client_fd];

	size_t endPos;

	while ((endPos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string msg = buffer.substr(0, endPos);
		buffer.erase(0, endPos + 2); //skip \r\n
		parseCommand(msg, client_fd);
	}
}

void Server::parseCommand(std::string msg, int client_fd){
	if (msg.empty())
		return;

	std::vector<std::string> params;
	std::string cmd;

	std::istringstream iss(msg);
	iss >> cmd;

	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

	std::string token;
	while (iss >> token)
	{
		if (token[0] == ':'){
			std::string start = token.substr(1);
			std::string rest;
			getline(iss, rest);
			params.push_back(start + rest);
			break;
		}
		if (token == "OK")
			std::cout << "OK received" << std::endl;
		// if (token == "KICK")
		// 	handleKick(client_fd, params);// it->kick(std::string client_to_eject, std::string channel_name, this->_clients, this->_channels)
		// else if  (token == "INVITE")
		// 	handleInvite(client_fd, params);
		// else if  (token == "TOPIC")
		// 	handleTopic(client_fd, params);
		// else if  (token == "MODE")
		// 	handleMode(client_fd, params);
		// else if  (token == "JOIN")
		// 	handleJoin(client_fd, params);
		// else if  (token == "PRIVMSG")
		// 	handlePrivmsg(client_fd, params);
		// else if  (token == "MODE")
		// 	handleMode(client_fd, params);
		else
		std::cerr << "CANT" << std::endl;
		// sendClientError(client_fd, "421,", cmd);
		(void)client_fd;
	}
}

void Server::run(){
	signal(SIGINT, handleSignal);
	signal(SIGTERM, handleSignal);
	signal(SIGQUIT, handleSignal);

	_fds.resize(1);
	_fds[0].fd = _server_socket;
	_fds[0].events = POLLIN;

	while(_isON)
	{
		if (poll(_fds.data(), _fds.size(), -1) < 0){
			if (errno == EINTR) continue;
				throw std::runtime_error(std::string("Error: poll: ") + std::strerror(errno));
		}
		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents == 0) // pas d'event
				continue;
			if (_fds[i].fd == _server_socket && _fds[i].revents & POLLIN) // & et non == car revents peut etre  = POLLIN mais pas que
				newClient();
			else if (_fds[i].revents & POLLIN)
				handleClient(i);
			else if (_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) // clean deco | error | invalid fd
			{
				removeClient(i);
				i--;
			}
		}
	}
	// cleanup();
}

Server::Server(int port, std::string password) : _port(port), _password(password)
{
	_isON = 1;
	_server_name = "IRC";
	try {
		createSocket();
		bindSocket();
		listenSocket();
		initErrorCodes();
		run();
	}
	catch (std::exception const &e){
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}
