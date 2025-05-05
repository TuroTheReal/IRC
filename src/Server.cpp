/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:35:44 by dsindres          #+#    #+#             */
/*   Updated: 2025/05/05 13:35:10 by artberna         ###   ########.fr       */
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
			return false;
	return true;
}

bool Server::isValidNickname(std::string nick){
	std::string prohibed;

	if (nick.size() > 20 || nick.empty())
		return false;

	for (char c = 0; c < 32; ++c)
		prohibed += c;
	prohibed += " ,*?!@.";

	for (size_t i = 0; i < nick.size(); i++){
		if (prohibed.find(nick[i]) != std::string::npos){
			std::cerr << "Invalid char in nick: '" << nick[i] << "'\n";
			return false;
		}
	}
	return true;
}

bool Server::isValidUsername(std::string user){
	std::string prohibed = " \r\n\0@*!";

	if (user.size() > 20 || user.empty())
		return false;

	for (size_t i = 0; i < user.size(); i++){
		if (prohibed.find(user[i]) != std::string::npos){
			std::cerr << "Invalid char in nick: '" << user[i] << "'\n";
			return false;
		}
	}
	return true;
}

void  Server::initErrorCodes(){
	_errorCodes["1"] = "ERREUR A REVOIR";
	_errorCodes["401"] = "No such nick/channel";
	_errorCodes["402"] = "No such server";
	_errorCodes["403"] = "No such channel";
	_errorCodes["409"] = "No origin specified";
	_errorCodes["410"] = "Invalid CAP subcommand";
	_errorCodes["411"] = "No recipient given (PRIVMSG)";
	_errorCodes["412"] = "No text to send";
	_errorCodes["421"] = "Unknown command";
	_errorCodes["431"] = "No nickname given";
	_errorCodes["432"] = "Erroneous nickname";
	_errorCodes["433"] = "Nickname is already in use";
	_errorCodes["441"] = "They aren't on that channel";
	_errorCodes["442"] = "You're not on that channel";
	_errorCodes["443"] = "Already on channel";
	_errorCodes["451"] = "You have not registered";
	_errorCodes["459"] = "Too many arguments";
	_errorCodes["461"] = "Not enough parameters";
	_errorCodes["462"] = "You may not reregister";
	_errorCodes["464"] = "Password incorrect";
	_errorCodes["465"] = "You are banned from this server";
	_errorCodes["471"] = "Cannot join channel (+l)";
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
	_errorCodes["601"] = "Invalid port number";
	_errorCodes["602"] = "Invalid file size";
	_errorCodes["603"] = "Not the destinator";
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
	std::string nickname = "*";
	std::vector<Client *>::iterator it = _clients.begin();

	for (; it != _clients.end(); it++)
	{
		if (client_fd == (*it)->get_socket())
		{
			std::string	client_nickname = (*it)->get_nickname();
			if (!client_nickname.empty() && client_nickname != "default")
				nickname = client_nickname;
			break;
		}
	}

	std::string errorMsg;
	std::map<std::string, std::string>::iterator ite = _errorCodes.find(key);

	if (ite != _errorCodes.end())
		errorMsg = ite->second;
	else
		errorMsg = "Unknow error";

	std::string to_send = ":" + _server_name + " " + key + " " + nickname;

	if (!cmd.empty())
		to_send += " " + cmd;

	to_send += ": " + errorMsg + "\r\n";

	ssize_t sent = send(client_fd, to_send.c_str(), to_send.length(), 0);
	if (sent < 0)
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
	std::cerr << "Error: client fd " << client_fd << " " << to_send << std::endl;
}

void Server::sendWelcome(int client_fd, Client* client){
	std::string nickname = client->get_nickname();

	std::string msg1 = ":" + _server_name + " 001 " + nickname + " :Welcome to the IRC Network " + nickname + "!" + client->get_username() + "@" + _host_name + "\r\n";
	ssize_t sent1 = send(client_fd, msg1.c_str(), msg1.length(), 0);
	if (sent1 < 0)
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));

	std::string msg2 = ":" + _server_name + " 002 " + nickname + " :Your host is " + _host_name + ", running version 1.0\r\n";
	ssize_t sent2 = send(client_fd, msg2.c_str(), msg2.length(), 0);
	if (sent2 < 0)
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));

	std::string msg3 = ":" + _server_name + " 003 " + nickname + " :This server was created today\r\n";
	ssize_t sent3 = send(client_fd, msg3.c_str(), msg3.length(), 0);
	if (sent3 < 0)
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));

	std::string msg4 = ":" + _server_name + " 004 " + nickname + " " + _host_name + " IRC 1.0, USER Mode: Chan operator. Channel Mode: +i +t +k +o +l\r\n";
	ssize_t sent4 = send(client_fd, msg4.c_str(), msg4.length(), 0);
	if (sent4 < 0)
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));

	// client->set_authentification(true);
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
	std::cout << "Client disconnected" << std::endl;
}

void Server::removeClientByFD(int client_fd){
	for (size_t i = 0; i < _fds.size(); i++){
		if (client_fd == _fds[i].fd){
			removeClient(i);
			return;
		}
	}
	std::cerr << "Error: fd " << client_fd << " not found in _fds" << std::endl;
}


void Server::cleanup(){
	if (!_clients.empty()) {
		for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
			delete *it;
		_clients.clear();
	}

	if (!_channels.empty()) {
		for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
			delete *it;
		_channels.clear();
	}

	if (!_fds.empty()) {
		for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); it++)
			safeClose(it->fd);
		_fds.clear();
	}

	if (!_pendingTransfers.empty()) {
		for (std::map<std::string, PendingTransfer>::iterator it = _pendingTransfers.begin(); it != _pendingTransfers.end(); it++)
			_pendingTransfers.erase(it);
		_pendingTransfers.clear();
	}
}

void Server::handleClient(size_t index){
	int client_fd = _fds[index].fd;
	char buffer[1024];

	ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read != 0)
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

	std::vector<std::string> params;
	std::string cmd;
	std::istringstream iss(msg);

	iss >> cmd;

	if (!cmd.empty() && cmd[0] == '!') {
		std::transform(cmd.begin() + 1, cmd.end(), cmd.begin() + 1, ::toupper);
		params.push_back(cmd);

		std::string token;
		while (iss >> token) {
			if (token[0] == ':') {
				std::string start = token.substr(1);
				std::string rest;
				getline(iss, rest);
				params.push_back(start + rest);
				break;
		    }
		    params.push_back(token);
		}

		Client* client = getClientByFD(client_fd);
		if (!client)
			throw std::runtime_error(std::string("client not found"));
		handleCommandBot(client_fd, params, client);
		return;
	}

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

	std::cout << "CMD dans parse == [" << cmd << "]" << std::endl;

	Client* client = getClientByFD(client_fd);
	if (!client)
		throw std::runtime_error(std::string("client not found"));

	if (cmd == "KICK") // 3 ou 4
		handleKick(client_fd, params, client);
	else if (cmd == "USER") // 5
		handleUser(client_fd, params, client);
	else if (cmd == "NICK") // 2
		handleNick(client_fd, params, client);
	else if (cmd == "PING") // 2 ou 3
		handlePing(client_fd, params);
	else if (cmd == "CAP") // 2 ou 3 // a verifier
		handleCap(client_fd, params, client);
	else if (cmd == "INVITE") // 3
		handleInvite(client_fd, params, client);
	else if (cmd == "TOPIC") // 2 ou 3
		handleTopic(client_fd, params, client);
	else if (cmd == "JOIN") // 2 ou 3
		handleJoin(client_fd, params, client);
	else if (cmd == "PRIVMSG") // 3
		handlePrivmsg(client_fd, params, client);
	else if (cmd == "MODE") // entre 2 et 5
		handleMode(client_fd, params, client);
	else if (cmd == "PASS") // 2
		handlePass(client_fd, params, client);
	else if (cmd == "QUIT")
		handleQuit(client_fd);
	else if (cmd == "XX")
		client->execute_command(params, _clients, _channels);
	else if (cmd == "XXX")
		client->execute_command(params, _clients, _channels);
	// else if (cmd == "SEND")
	// 	handleSend(client_fd, params, client);
	// else if (cmd == "ACCEPT")
	// 	handleAccept(client_fd, params, client);
	// else if (cmd == "DECLINE")
	// 	handleDecline(client_fd, params, client);
	else
		sendClientError(client_fd, "421", cmd);
}

void Server::handleCommandBot(int client_fd, std::vector<std::string> params, Client* client){
	std::string to_ret;
	std::string cmd = params[0].substr(1);

	if (cmd == "TIME"){
		time_t now = time(0);
		char* dt = ctime(&now);
		to_ret = "Current time: " + std::string(dt);
		if (!to_ret.empty() && to_ret[to_ret.length()-1] == '\n') {
			to_ret.erase(to_ret.length()-1);
		}
	}
	else if (cmd == "WEATHER")
		to_ret = "Today's weather: 25°C, sunny.";
	else if (cmd == "HELP")
		to_ret = "Commands are : !HELP, !TIME & !WEATHER";
	else if (cmd == "WHOAMI"){
		if (!client->get_nickname().empty()) // remplacer par booleen has_nick
			to_ret = "Your nick is: " + client->get_nickname();
		if (!client->get_username().empty()) // remplacer par booleen has_user
			to_ret += "\nYour user is: " + client->get_username();
	}
	else {
		sendClientError(client_fd, "421", cmd + " :Use !HELP to see all the bot commands");
		return;
	}

	std::string response = ":" + _server_name + "_bot PRIVMSG " + client->get_nickname() + " :" + to_ret + "\r\n";
	ssize_t sent = send(client_fd, response.c_str(), response.size(), 0);
	if (sent < 0)
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
}

Client* Server::getClientByFD(int client_fd){
	std::vector<Client*>::iterator it = _clients.begin();

	for (; it != _clients.end(); it++){
		if ((*it)->get_socket() == client_fd)
			return *it;
	}
	return NULL;
}

// void Server::handleSend(int client_fd, std::vector<std::string> params, Client* client){
// 	// if (!client->isRegistered()) {
// 	// 	sendClientError(client_fd, "451", params[0]);
// 	// 	return;
// 	// }

// 	if (params.size() != 5){
// 		if (params.size() < 5)
// 			sendClientError(client_fd, "461", params[0]);
// 		else if (params.size() > 5)
// 			sendClientError(client_fd, "459", params[0]);
// 		return;
// 	}

// 	int port;
// 	try {
// 		port = std::atoi(params[2].c_str());
// 		if (port <= 0 || port > 65535)
// 			throw std::exception();
// 	}
// 	catch (...){
// 		sendClientError(client_fd, "601", params[0]);
// 		return;
// 	}

// 	ssize_t filesize;
// 	try {
// 		filesize = std::atol(params[4].c_str());
// 		if (filesize < 0)
// 			throw std::exception();
// 	}
// 	catch (...){
// 		sendClientError(client_fd, "602", params[0]);
// 		return;
// 	}

// 	std::string nick = params[1];
// 	Client* target = NULL;
// 	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
// 		if ((*it)->get_nickname() == nick) {
// 			target = *it;
// 			break;
// 		}
// 	}

// 	if (!target) {
// 		sendClientError(client_fd, "401", nick);
// 		return;
// 	}

// 	std::string filename = params[3];

// 	struct sockaddr_in client_addr;
// 	socklen_t addr_len = sizeof(client_addr);
// 	getpeername(client_fd, (struct sockaddr*)&client_addr, &addr_len);
// 	uint32_t ip_num = ntohl(client_addr.sin_addr.s_addr);

// 	std::string transfer_id = client->get_nickname() + "_" + nick + "_" + filename + "_" + std::to_string(time(NULL));

// 	PendingTransfer transfer;
// 	transfer.sender_nick = client->get_nickname();
// 	transfer.receiver_nick = nick;
// 	transfer.filename = filename;
// 	transfer.ip_address = ip_num;
// 	transfer.port = port;
// 	transfer.filesize = filesize;

// 	_pendingTransfers[transfer_id] = transfer;

// 	std::stringstream dcc_msg;

// 	dcc_msg << "\001DCC SEND " << filename << " " << ip_num << " " << port << " " << filesize << " " << transfer_id << "\001";

// 	std::string privmsg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + _host_name + " PRIVMSG " + nick + " :" + dcc_msg.str() + "\r\n";
// 	ssize_t sent = send(target->get_socket(), privmsg.c_str(), privmsg.length(), 0);
// 	if (sent < 0)
// 		throw std::runtime_error(std::string("send: ") + std::strerror(errno));

// 	std::string info_msg = ":" + _server_name + " NOTICE " + nick + " :Use ACCEPT " + transfer_id + " to accept or DECLINE " + transfer_id + " to refuse the transfer.\r\n";
// 	sent = send(target->get_socket(), info_msg.c_str(), info_msg.length(), 0);
// 	if (sent < 0)
// 		throw std::runtime_error(std::string("send: ") + std::strerror(errno));

// 	std::string confirm = ":" + _server_name + " NOTICE " + client->get_nickname() + " :DCC SEND request sent to " + nick + " (ID: " + transfer_id + ")\r\n";
// 	sent = send(client_fd, confirm.c_str(), confirm.length(), 0);
// 	if (sent < 0)
// 		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
// }

// void Server::handleAccept(int client_fd, std::vector<std::string> params, Client* client){
// 	if (params.size() != 2) {
// 		sendClientError(client_fd, "461", params[0]);
// 		return;
// 	}

// 	std::string transfer_id = params[1];

// 	std::map<std::string, PendingTransfer>::iterator it = _pendingTransfers.find(transfer_id);
// 	if (it == _pendingTransfers.end()){
// 		sendClientError(client_fd, "461", params[0]);
// 	}

// 	PendingTransfer& transfer = it->second;
// 	if (transfer.receiver_nick != client->get_nickname()){
// 		sendClientError(client_fd, "603", params[0]);
// 		return;
// 	}

// 	uint32_t ip = transfer.ip_address;
// 	std::string ip_str = std::to_string((ip >> 24) & 0xFF) + "." +
// 							std::to_string((ip >> 16) & 0xFF) + "." +
// 							std::to_string((ip >> 8) & 0xFF) + "." +
// 							std::to_string(ip & 0xFF);

// 	std::string accept_msg = ":" + _server_name + " NOTICE " + client->get_nickname() +
// 							" :Connecton init to " + ip_str + ":" +
// 							std::to_string(transfer.port) + " to receive " +
// 							transfer.filename + "\r\n";

// 	ssize_t sent = send(client_fd, accept_msg.c_str(), accept_msg.length(), 0);
// 	if (sent < 0)
// 		throw std::runtime_error(std::string("send: ") + std::strerror(errno));

// 	Client *sender = NULL;
// 	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
// 		if ((*it)->get_nickname() == transfer.sender_nick) {
// 			sender = *it;
// 			break;
// 		}
// 	}

// 	if (sender) {
// 		std::string sender_msg = ":" + _server_name + " NOTICE " + sender->get_nickname() +
// 								" :" + client->get_nickname() + " has accepted to receive " +
// 								transfer.filename + "\r\n";

// 		sent = send(sender->get_socket(), sender_msg.c_str(), sender_msg.length(), 0);
// 		if (sent < 0)
// 			throw std::runtime_error(std::string("send: ") + std::strerror(errno));
// 	}

// 	_pendingTransfers.erase(transfer_id);
// }

// void Server::handleDecline(int client_fd, std::vector<std::string> params, Client* client){
// 	if (params.size() != 2) {
// 		sendClientError(client_fd, "461", params[0]);
// 		return;
// 	}

// 	std::string transfer_id = params[1];

// 	std::map<std::string, PendingTransfer>::iterator it = _pendingTransfers.find(transfer_id);
// 	if (it == _pendingTransfers.end()){
// 		sendClientError(client_fd, "461", params[0]);
// 	}

// 	PendingTransfer& transfer = it->second;
// 	if (transfer.receiver_nick != client->get_nickname()){
// 		sendClientError(client_fd, "603", params[0]);
// 		return;
// 	}

// 	std::string decline_msg = ":" + _server_name + " NOTICE " + client->get_nickname() +
// 								" :You refuse the transfert of " + transfer.filename +
// 								" from " + transfer.sender_nick + "\r\n";

// 	ssize_t sent = send(client_fd, decline_msg.c_str(), decline_msg.length(), 0);
// 	if (sent < 0)
// 		throw std::runtime_error(std::string("send: ") + std::strerror(errno));

// 	Client *sender = NULL;
// 	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
// 		if ((*it)->get_nickname() == transfer.sender_nick) {
// 			sender = *it;
// 			break;
// 		}
// 	}

// 	if (sender) {
// 		std::string sender_msg = ":" + _server_name + " NOTICE " + sender->get_nickname() +
// 									" :" + client->get_nickname() + " has refuse to receive " +
// 									transfer.filename + "\r\n";

// 		sent = send(sender->get_socket(), sender_msg.c_str(), sender_msg.length(), 0);
// 		if (sent < 0)
// 			throw std::runtime_error(std::string("send: ") + std::strerror(errno));
// 	}

// 	_pendingTransfers.erase(transfer_id);
// }

void Server::handleKick(int client_fd, std::vector<std::string> params, Client* client){
	// if (!client->isRegistered()) {
	// 	sendClientError(client_fd, "451", params[0]);
	// 	return;
	// }

	if (params.size() < 3 || params.size() > 4 ){
		if (params.size() < 3)
			sendClientError(client_fd, "461", params[0]);
		if (params.size() > 4)
			sendClientError(client_fd, "459", params[0]);
		return;
	}

	int res = client->execute_command(params, _clients, _channels);
	if (res != 0 && res != 11){
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
		// Si canal inexistant : 403
		// Si utilisateur pas sur le canal : 441
		// Si non opérateur : 482
	}
}

void Server::handleUser(int client_fd, std::vector<std::string> params, Client* client){
	// if (!client->hasPassword()) {
	// 	sendClientError(client_fd, "464", params[0]);
	// 	return;
	// }

	if (params.size() != 5){
		sendClientError(client_fd, "461", params[0]);
		return;
	}

	if (!isValidUsername(params[1])){
		sendClientError(client_fd, "432", params[0]);
		return;
	}

	int res = client->execute_command(params, _clients, _channels);
	(void)res;
	// if (res != 0 && res != 11){
	// 	std::ostringstream oss;
	// 	oss << res;
	// 	sendClientError(client_fd, oss.str() ,params[0]);
		// Si déjà enregistré : 462
		// return;
	// }

	// client->set_user(true);
	// if (client->isRegistered());
	// 	sendWelcome(client_fd, client);
}

void Server::handleNick(int client_fd, std::vector<std::string> params, Client* client){
	// if (!client->hasPassword()) {
	// 	sendClientError(client_fd, "464", params[0]);
	// 	return;
	// }

	if (params.size() != 2){
		if (params.size() < 2)
			sendClientError(client_fd, "431", params[0]);
		if (params.size() > 2)
			sendClientError(client_fd, "459", params[0]);
		return ;
	}

	if (!isValidNickname(params[1])){
		sendClientError(client_fd, "432", params[0]);
		return ;
	}

	int res = client->execute_command(params, _clients, _channels);
	(void)res;
	// if (res != 0 && res != 11){
	// 	std::ostringstream oss;
	// 	oss << res;
	// 	sendClientError(client_fd, oss.str() ,params[0]);
	// Si pseudo déjà utilisé : 433
	// return;
	// }

	// client->set_nick(true);
	// if (client->isRegistered());
	// 	sendWelcome(client_fd, client);
	sendWelcome(client_fd, client);
}

void Server::handlePing(int client_fd, std::vector<std::string> params){
	if (params.size() != 2){
		sendClientError(client_fd, "409", params[0]);
		return;
	}
	std::string response = ":" + _server_name + " PONG " + _server_name + " :" + params[1] + "\r\n";

	ssize_t sent = send(client_fd, response.c_str(), response.length(), 0);
	if (sent < 0)
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
	std::cout << "Sent PONG response to client " << client_fd << std::endl;
}

void Server::handleCap(int client_fd, std::vector<std::string> params, Client* client){
	if (params.size() < 2 || params.size() > 3 ){
		if (params.size() < 2)
			sendClientError(client_fd, "461", params[0]);
		if (params.size() > 3)
			sendClientError(client_fd, "459", params[0]);
		return;
	}
	std::string option = params[1];
	for (size_t i = 0; i < option.size(); i++)
		option[i] = toupper(option[i]);

	if (option == "LS"){
		std::string response = ":" + _host_name + " CAP " + client->get_nickname() + " LS :\r\n";
		ssize_t sent = send (client_fd, response.c_str(), response.length(), 0);
		if (sent < 0)
			throw std::runtime_error(std::string("send: ") + std::strerror(errno));
	}
	else if (option == "REQ"){
		std::string response = ":" + _host_name + " CAP " + client->get_nickname() + " ACK :\r\n";
		ssize_t sent = send(client_fd, response.c_str(), response.length(), 0);
		if (sent < 0)
			throw std::runtime_error(std::string("send: ") + std::strerror(errno));
	}
	else if (option == "END"){
		return;
	}
	else
		sendClientError(client_fd, "410", option);
}

void Server::handleInvite(int client_fd, std::vector<std::string> params, Client* client){
	// if (!client->isRegistered()) {
	// 	sendClientError(client_fd, "451", params[0]);
	// 	return;
	// }

	if (params.size() != 5){
		sendClientError(client_fd, "461", params[0]);
		return;
	}
	// Si utilisateur inexistant : 401
	// Si canal inexistant : 403
	// Si non opérateur (si +i) : 482
	(void)client;
}

void Server::handleTopic(int client_fd, std::vector<std::string> params, Client* client){
	// if (!client->isRegistered()) {
	// 	sendClientError(client_fd, "451", params[0]);
	// 	return;
	// }

	if (params.size() < 2 || params.size() > 3 ){
		if (params.size() < 2)
			sendClientError(client_fd, "461", params[0]);
		if (params.size() > 3)
			sendClientError(client_fd, "459", params[0]);
		return;
	}
	// Si canal inexistant : 403
	// Si utilisateur pas sur le canal : 442
	// Si non opérateur (si +t) : 482
	(void)client;
}

void Server::handleJoin(int client_fd, std::vector<std::string> params, Client* client){
	// if (!client->isRegistered()) {
	// 	sendClientError(client_fd, "451", params[0]);
	// 	return;
	// }

	if (params.size() < 2 || params.size() > 3 ){
		if (params.size() < 2)
			sendClientError(client_fd, "461", params[0]);
		if (params.size() > 3)
			sendClientError(client_fd, "459", params[0]);
		return;
	}

	if (!isValidChannel(params[1])){
		sendClientError(client_fd, "476", params[1]);
		return;
	}

	int res = client->execute_command(params, _clients, _channels);
	if (res == 11){
		std::string new_channel_name = params[1];
		new_channel_name.erase(0,1);
		Channel* new_channel = new Channel(new_channel_name, client);
		client->set_operator(true);
		client->add_channel_operator(new_channel);
		this->_channels.push_back(new_channel);
		int res2 = client->execute_command(params, _clients, _channels);
		if (res2 != 0)
		{
			std::ostringstream oss;
			oss << res2;
			sendClientError(client_fd, oss.str() ,params[0]);
		}
	}
	else if (res != 0 && res != 11){
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);		// Si canal inexistant : 403
	}
		// Si canal complet : 471
		// Si canal sur invitation : 473
}

void Server::handlePrivmsg(int client_fd, std::vector<std::string> params, Client* client){
	// if (!client->isRegistered()) {
	// 	sendClientError(client_fd, "451", params[0]);
	// 	return;
	// }

	if (params.size() != 3){
		sendClientError(client_fd, "411", params[0]); // ereur a check
		return;
	}
	int res = client->execute_command(params, _clients, _channels);// ereur a check
	if (res != 0 && res != 11)
	{
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
		// Si destinataire absent : 401
		// Si pas de texte : 412
	}
}

void Server::handleMode(int client_fd, std::vector<std::string> params, Client* client){
	// if (!client->isRegistered()) {
	// 	sendClientError(client_fd, "451", params[0]);
	// 	return;
	// }

	if (params.size() < 2 || params.size() > 5 ){
		if (params.size() < 2)
			sendClientError(client_fd, "461", params[0]);
		if (params.size() > 5)
			sendClientError(client_fd, "459", params[0]);
		return;
	}
	// Si mode inconnu : 472
	// Si mode utilisateur inconnu : 501
	// Si tentative de modifier le mode d'un autre utilisateur : 502
	(void)client;
}

void Server::handlePass(int client_fd, std::vector<std::string> params, Client* client){
	if (params.size() != 2){
		sendClientError(client_fd, "461", params[0]);
		return;
	}

	// if (client->has_password()){
	// 	sendClientError(client_fd, "462", params[0]);
	// 	return;
	// }

	if (params[1] != _password){
		sendClientError(client_fd, "464", params[0]);
		return;
	}
	// client->set_password(true);
	(void)client;
}

void Server::handleQuit(int client_fd){
	removeClientByFD(client_fd);
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
		cleanup();
		throw;
	}
}

Server::~Server(){
	std::cout << "Server closed" << std::endl;
	cleanup();
}


/*******************************************************************/


// demander qu est ce quune registration complete, et si apres quelle soit complete on peut encore faire les commandes pour se register ?
// USERHOST cest quoi ?
// check si cleanup avant throw || dans le catch || apres pour clean correctement apres un erreur