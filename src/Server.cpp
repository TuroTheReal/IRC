/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:35:44 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/23 14:09:16 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Server::createSocket(){
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(fd);
		throw std::runtime_error(std::string("fcntl: ") + std::strerror(errno));
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
		throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
}

void Server::listenSocket(){
	if (listen(_server_socket, 5) < 0) {
		throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
	}
}

volatile sig_atomic_t _isON = 1;

void handleSignal(int signum){
	(void)signum;
	_isON = 0;
}

bool Server::isValidChannel(std::string chan){
	const std::string not_in_first = "!#&+";
	const std::string not_inside = " ,:\x07";
	if (not_in_first.find(chan[0]) == std::string::npos)
		return false;
	if (chan.size() > 50)
		return false;
	for (size_t i = 1; i < chan.size(); i++)
		if (not_inside.find(chan[i]) != std::string::npos)
			return 0;
	return true;
}

void  Server::initErrorCodes(){
	_errorCodes["401"] = ": No such nick/channel";
	_errorCodes["403"] = ": No such channel";
	_errorCodes["421"] = ": Unknown command";
	_errorCodes["431"] = ": No nickname given";
	_errorCodes["432"] = ": Erroneous nickname";
	_errorCodes["433"] = ": Nickname is already in use";
	_errorCodes["441"] = ": They aren't on that channel";
	_errorCodes["442"] = ": You're not on that channel";
	_errorCodes["461"] = ": Not enough parameters";
	_errorCodes["462"] = ": Too many arguments";
	_errorCodes["472"] = ": Unknown mode char";
	_errorCodes["473"] = ": Cannot join channel (+i)";
	_errorCodes["474"] = ": Cannot join channel (+b)";
	_errorCodes["475"] = ": Cannot join channel (+k)";
	_errorCodes["476"] = ": Bad channel mask";
	_errorCodes["481"] = ": Permission Denied (You're not an IRC operator)";
	_errorCodes["482"] = ": You're not channel operator";
	_errorCodes["484"] = ": Your connection is restricted";
	_errorCodes["501"] = ": Unknown MODE flag";
	_errorCodes["502"] = ": Cannot change mode for other users";
}

void	Server::getHostName(){
	char hostname[1024];
	if (gethostname(hostname, sizeof(hostname)) == -1){
		std::cerr << "Error: gethostname: " << strerror(errno) << std::endl;
		_host_name = "localhost";
	}
	_host_name = std::string(hostname);
}

void Server::sendClientError(int client_fd, const std::string& key, const std::string& cmd){
	std::vector<Client *>::iterator it = _clients.begin();
	std::string nickname;

	for (; it != _clients.end(); it++)
	{
		if (client_fd == (*it)->get_socket())
			nickname = (*it)->get_nickname();
		else
			nickname = "*";
	}

	if (nickname == "default")
		nickname = "*";

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
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
	std::cerr << "Error: client fd " << client_fd << ": " << to_send << std::endl;
}

void Server::newClient(){
	std::cout << "Connection received from new" << std::endl;

	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	int client_fd = accept(_server_socket, (struct sockaddr*)&client_addr, &addr_len);
	if (client_fd < 0){
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw std::runtime_error(std::string("accept: ") + std::strerror(errno));
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(client_fd);
			throw std::runtime_error(std::string("fcntl: ") + std::strerror(errno));
	}

	struct pollfd client;
	client.fd = client_fd;
	client.events = POLLIN;
	client.revents = 0;

	_fds.push_back(client);

	Client* new_client = new Client(client_fd);
	_clients.push_back(new_client);
}

void safeClose(int& fd)
{
	if (fd >= 0)
	{
		if (close(fd) == -1)
			std::cerr << "Error: close on fd " << fd << ": " << strerror(errno) << std::endl;
		fd = -1;
	}
}

void Server::removeClient(size_t index){
	int fd = _fds[index].fd;
	safeClose(_fds[index].fd);
	_fds.erase(_fds.begin() + index);

	std::vector<Client*>::iterator it = _clients.begin();
	std::vector<Client*>::iterator ite = _clients.end();

	for (; it != ite; it++)
	{
		if ((*it)->get_socket() == fd)
		{
			delete *it;
			_clients.erase(it);
			break;
		}
	}
	// clean data dans les differentes classes : _client.erase(_fds[index].fd) || _client.erase(_fds[index])
	std::cout << "Client disconnected from remove" << std::endl;
}

void Server::cleanup(){
	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); it++)
		safeClose(it->fd);
	// _fds.clear(); // pas sur
	// delete [] _clients; //free client
}

void Server::handleClient(size_t index){
	int client_fd = _fds[index].fd;
	char buffer[1024];

	ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read == 0)
			std::cout << "Client disconnected from handle" << std::endl;
		else
			throw std::runtime_error(std::string("recv: ") + std::strerror(errno));
		removeClient(index);
		return ;
	}
	buffer[bytes_read] = '\0';
	std::cout << "Données brutes reçues: [" << buffer << "]" << std::endl;
	_clientBuffers[client_fd] += std::string(buffer);
	processClientBuffer(client_fd);
}

void Server::processClientBuffer(int client_fd){
	if (_clientBuffers.find(client_fd) == _clientBuffers.end())
		throw std::runtime_error(std::string("client not found"));

	std::string& buffer = _clientBuffers[client_fd];
	std::cout << "Données dans buffer client: [" << buffer << "]" << std::endl;
	size_t endPos;

	while ((endPos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string msg = buffer.substr(0, endPos);
		buffer.erase(0, endPos + 2); //skip \r\n
		std::cout << "Message complet extrait: [" << msg << "]" << std::endl;
		parseCommand(msg, client_fd);
	}
}

void Server::parseCommand(std::string msg, int client_fd){
	std::cout << "Message brut reçu: [" << msg << "]" << std::endl;
	if (msg.empty())
		return;

	std::cout << "Recu dans parseCommand" << std::endl;

	std::vector<std::string> params;
	std::string cmd;
	std::istringstream iss(msg);

	iss >> cmd;
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

	params.push_back(cmd);
	std::string token;
	while (iss >> token)
	{
		if (token[0] == ':'){
			std::string start = token.substr(1);
			std::string rest;
			getline(iss, rest);
			if ((start + rest).empty())
				break;
			params.push_back(start + rest);
			break;
		}
		params.push_back(token);
	}

	std::cout << "CMD == [" << cmd << "]" << std::endl;

	Client* client = getClientByFD(client_fd);
	if (!client)
		throw std::runtime_error(std::string("client not found"));

	display_all_clients();
	display_all_channels();
	if (cmd == "KICK") // 3 ou 4
		handleKick(client_fd, params, client);
	else if (cmd == "USER") // 5
		handleUser(client_fd, params, client);
	else if (cmd == "PING")
		handlePing(client_fd, params, client);
	else if (cmd == "NICK") // 2
		handleNick(client_fd, params, client);
	else if (cmd == "CAP")
		handleCap(client_fd, params, client);
	else if (cmd == "INVITE") // 3
		handleInvite(client_fd, params, client);
	else if (cmd == "TOPIC") // 2 ou 3
		handleTopic(client_fd, params, client);
	else if (cmd == "JOIN") // 2 ou 3
		handleJoin(client_fd, params, client);
	else if (cmd == "PRIVMSG") // 3
		handlePrivmsg(client_fd, params, client);
	else if (cmd == "MODE") // entre 3 et 5
		handleMode(client_fd, params, client);
	else if (cmd == "PASS") // 2
		handlePass(client_fd, params, client);
	else if (cmd == "QUIT") // 1 ou 2
		handleQuit(client_fd, params, client);
	else
		sendClientError(client_fd, "421", cmd);
	(void)client_fd;
	// display_all();
}

Client* Server::getClientByFD(int client_fd){
	std::vector<Client*>::iterator it = _clients.begin();

	for (; it != _clients.end(); it++){
		if ((*it)->get_socket() == client_fd)
			return *it;
	}
	return NULL;
}

void Server::handleKick(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "KICK command (à implémenter)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;
}

void Server::handleUser(int client_fd, std::vector<std::string> params, Client* client){
	// std::string response = ":" + _server_name + " 002 * :Your host is " + _host_name + "\r\n";
	// send(client_fd, response.c_str(), response.length(), 0);
	// (void)client_fd;
	// (void)client;
	// (void)params;
	if (params.size() != 5)
		sendClientError(client_fd, "461", params[0]);

	std::vector<std::string>::iterator it = params.begin();
	for (; it != params.end(); it++)
		std::cout << *it << "\n";
	int res = client->execute_command(params, _clients, _channels);
	std::cout << res << std::endl;
	std::cout << "USER = " << client->get_username() << std::endl;
	(void)client_fd;
}

void Server::handlePing(int client_fd, std::vector<std::string> params, Client* client){
	std::string response = "PONG ";
	if (!params.empty()) {
		response += params[0];
	}
	response += "\r\n";
	send(client_fd, response.c_str(), response.length(), 0);
	std::cout << "PONG" << std::endl;
	(void)client;
}

void Server::handleNick(int client_fd, std::vector<std::string> params, Client* client){
	if (params.size() != 2)
	{
		if (params.size() < 2)
			sendClientError(client_fd, "431", params[0]);
		if (params.size() > 2)
			sendClientError(client_fd, "462", params[0]);
	}

	std::string nick = params[1];
	std::cout << "NICK défini: " << nick << std::endl;
	// Confirmation au client
	std::string response = ":" + _server_name + " 001 " + nick + " :Welcome to the IRC server\r\n";
	send(client_fd, response.c_str(), response.length(), 0);
	(void)client;
}

void Server::handleCap(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "CAP command (à implémenter)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;
}

void Server::handleInvite(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "INVITE command (à implémenter)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;
}

void Server::handleTopic(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "TOPIC command (à implémenter)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;
}

void Server::handleJoin(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "JOIN command (en cours)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;

	if (params.size() < 2 && params.size() > 3)
		sendClientError(client_fd, "461", params[0]);
	if (!isValidChannel(params[1]))
		sendClientError(client_fd, "476", params[1]);
	int res = client->execute_command(params, _clients, _channels);
	if (res == 11)
	{
		std::string new_channel_name = params[1];
		new_channel_name.erase(0,1);
		Channel* new_channel = new Channel(new_channel_name, client);
		client->set_operator(true);
		client->add_channel_operator(new_channel);
		this->_channels.push_back(new_channel);
		int res2 = client->execute_command(params, _clients, _channels);
		// if (res2 != 0)
		// 	sendClientError();
		(void)res2;
	}
	display_all_clients();
	display_all_channels();
}

void Server::handlePrivmsg(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "PRIVMSG command (à implémenter)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;

}

void Server::handleMode(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "MODE command (à implémenter)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;
}

void Server::handlePass(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "PASS command (à implémenter)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;
}

void Server::handleQuit(int client_fd, std::vector<std::string> params, Client* client){
	std::cout << "QUIT command (à implémenter)" << std::endl;
	(void)client_fd;
	(void)client;
	(void)params;
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
				throw std::runtime_error(std::string("poll: ") + std::strerror(errno));
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
	cleanup();
}

Server::Server(int port, std::string password) : _port(port), _password(password), _server_name("IRC"){
	_isON = 1;
	try {
		createSocket();
		bindSocket();
		listenSocket();
		initErrorCodes();
		getHostName();
		run();
	}
	catch (std::exception const &e){
		throw;
	}
}

void Server::display_all_channels()
{
    std::vector<Channel*>::iterator it = _channels.begin();
    std::cout << std::endl;
    while(it != _channels.end())
    {
        Client *ope = (*it)->get_operator();
        std::cout << "  Channel : " << (*it)->get_name() << std::endl;
        std::cout << "   operator --> " << ope->get_username() << std::endl;
        (*it)->get_all_clients();
        std::cout << std::endl;
        it++;
    }
    std::cout << "  -------- end of channels ----------" << std::endl << std::endl;
}


void Server::display_all_clients()
{
    std::vector<Client*>::iterator ite = _clients.begin();
    std::cout << std::endl;
    while(ite != _clients.end())
    {
        std::cout << "  Client : " << (*ite)->get_username() << std::endl;
        (*ite)->get_operator();
        (*ite)->get_channel();
        (*ite)->get_invitation();
        std::cout << std::endl;
        ite++;
    }
    std::cout << "  -------- end of clients ----------" << std::endl << std::endl;
}