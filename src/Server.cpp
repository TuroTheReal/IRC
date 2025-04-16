/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:35:44 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/16 15:15:31 by artberna         ###   ########.fr       */
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

void Server::newClient(){
	std::cout << "Connection received" << std::endl;

	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	int client_fd = accept(_server_socket, (struct sockaddr*)&client_addr, &addr_len);

	if (client_fd < 0){
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw std::runtime_error(std::string("Error: accept: ") + std::strerror(errno));
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(client_fd);
			throw std::runtime_error(std::string("Error: fcntl: ") + std::strerror(errno));
	}

	struct pollfd client;
	client.fd = client_fd;
	client.events = POLLIN;
	client.revents = 0;

	_fds.push_back(client);

	Client* new_client = new Client(client_fd); // pourquoi allocation ?? voir avec le D
	_clients.push_back(new_client);
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
	std::string msg(buffer);

	std::vector<Client*>::iterator it = _clients.begin();
	std::vector<Client*>::iterator ite = _clients.end();

	int res;

	for (; it != ite; it++)
	{
		if (it.get_socket() == client_fd)
			res = it.received_message(buffer, _clients, _channels);
	}

	std::ostringstream oss;

	if (it == ite)
	{
		oss << "Error: Cannot find client related to fd " << client_fd;
		throw std::runtime_error(oss.str());
	}

	if (res < 0)
	{
		oss << "Error while with the message of fd " << client_fd;
		throw std::runtime_error(oss.str());
	}
}

void Server::removeClient(size_t index){
	close(_fds[index].fd);
	_fds.erase(_fds.begin() + index);
	// clean data dans les differentes classes : _client.erase(_fds[index].fd) || _client.erase(_fds[index])
	std::cout << "Client disconnected" << std::endl;
}

void Server::run(){
	_fds.resize(1);
	_fds[0].fd = _server_socket;
	_fds[0].events = POLLIN;

	while(1)
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
	try {
		createSocket();
		bindSocket();
		listenSocket();
		run();
	}
	catch (std::exception const &e){
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}
