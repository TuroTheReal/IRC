/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:35:44 by dsindres          #+#    #+#             */
/*   Updated: 2025/05/07 15:00:38 by artberna         ###   ########.fr       */
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
	if (chan.empty())
		return false;
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

bool Server::isNotTakenChannel(std::string chan){
	std::vector<Channel*>::iterator it = _channels.begin();
	std::vector<Channel*>::iterator ite = _channels.end();

	for (; it != ite; it++){
		if (chan == (*it)->get_name())
			return false;
	}
	return true;
}

Client* Server::findClientByNickname(const std::string& nickname) {
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if ((*it)->get_nickname() == nickname) {
			return *it;
		}
	}
	return NULL;
}

// Remplacement de la fonction to_string pour C++98
std::string toString(size_t value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string toString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string toString(long value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void  Server::initErrorCodes(){
	_errorCodes["331"] = "No topic is set";
	_errorCodes["401"] = "No such nick/channel";
	_errorCodes["402"] = "No such server";
	_errorCodes["403"] = "No such channel";
	_errorCodes["409"] = "No origin specified";
	_errorCodes["410"] = "Invalid CAP subcommand";
	_errorCodes["411"] = "No recipient given (PRIVMSG)";
	_errorCodes["412"] = "No text to send";
	_errorCodes["421"] = "Unknown command";
	_errorCodes["430"] = "Erroneous username";
	_errorCodes["431"] = "No nickname given";
	_errorCodes["432"] = "Erroneous nickname";
	_errorCodes["433"] = "Nickname is already in use";
	_errorCodes["434"] = "Channelname is already in use";
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
	_errorCodes["604"] = "No transfer for this id";
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

	client->set_autentification(true);
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

	for (; it != ite; it++) {
		if ((*it)->get_socket() == fd){

			std::vector<Channel*> channel_to_delete = (*it)->supp_channel();
			if (!channel_to_delete.empty()){
				for (std::vector<Channel*>::iterator del_it = channel_to_delete.begin(); del_it != channel_to_delete.end(); del_it++){
					std::vector<Channel*>::iterator it2 = _channels.begin();
					while (it2 != _channels.end()) {
						if ((*it2)->get_name() == (*del_it)->get_name()){
							delete *it2;
							_channels.erase(it2);
							break;
						}
						else
						it2++;
					}
				}
			}
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

	_fileTransfers.clear();

	for (std::map<int, TransferSession>::iterator it = _activeTransfers.begin(); it != _activeTransfers.end(); ++it) {
		if (close(it->first) < 0)
			std::cerr << "Error: close on fd " << it->first << ": " << strerror(errno) << std::endl;
		// Remove from master set if it's still there
		FD_CLR(it->first, &_master_read_fds);
	}

	_activeTransfers.clear();

	// Reset file descriptor sets
	FD_ZERO(&_master_read_fds);
	_max_fd = 0;
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
	else if (cmd == "SEND")
		handleSend(client_fd, params, client);
	else if (cmd == "UPLOAD")
		handleUpload(client_fd, params, client);
	else if (cmd == "ACCEPT")
		handleAccept(client_fd, params, client);
	else if (cmd == "DECLINE")
		handleDecline(client_fd, params, client);
	else
		sendClientError(client_fd, "421", cmd);
}

void Server::handleCommandBotPriv(int client_fd, std::vector<std::string> params, Client* client){
	std::string to_ret;
	std::string cmd = params[2].substr(1);

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
		if (client->get_bool_nick() == true)
			to_ret = "Your nick is: " + client->get_nickname();
		else
			to_ret = "No nick given";

		if (client->get_bool_user() == true)
			to_ret += "\nYour user is: " + client->get_username();
		else
			to_ret += "\nNo user given";
	}
	else
		to_ret += "Unknow command :Use !HELP to see all the bot commands";

	std::string response = ":" + _server_name + "_bot PRIVMSG " + params[1] + " :" + to_ret + "\r\n";
	ssize_t sent = send(client_fd, response.c_str(), response.size(), 0);
	if (sent < 0)
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
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
		if (client->get_bool_nick() == true)
			to_ret = "Your nick is: " + client->get_nickname();
		else
			to_ret = "No nick given";
		if (client->get_bool_user() == true)
			to_ret += "\nYour user is: " + client->get_username();
		else
			to_ret += "\nNo user given";
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


void Server::handleUpload(int client_fd, std::vector<std::string> params, Client* client) {
    // Format : UPLOAD transfer_id data_chunk
    if (params.size() < 3) {
        sendClientError(client_fd, "461", params[0]);
        return;
    }

    std::string transfer_id = params[1];
    std::string data_chunk = params[2];

    // Récupérer le transfert correspondant
    std::map<std::string, FileTransfer>::iterator it = _fileTransfers.find(transfer_id);
    if (it == _fileTransfers.end()) {
        sendClientError(client_fd, "604", params[0]);
        return;
    }

    FileTransfer& transfer = it->second;

    // Vérifier que le client est bien l'émetteur
    if (transfer.sender_nick != client->get_nickname()) {
        sendClientError(client_fd, "603", params[0]);
        return;
    }

    // Ajouter les données au buffer
    transfer.data.insert(transfer.data.end(), data_chunk.begin(), data_chunk.end());

    // Vérifier si le transfert est complet
    if (transfer.data.size() >= transfer.filesize) {
        transfer.transfer_complete = true;

        // Notifier l'émetteur que l'upload est terminé
        std::string upload_complete = ":" + _server_name + " NOTICE " + client->get_nickname() +
                                     " :Upload du fichier " + transfer.filename + " terminé. En attente d'acceptation.\r\n";
        ssize_t sent = send(client_fd, upload_complete.c_str(), upload_complete.length(), 0);
        if (sent < 0)
            throw std::runtime_error(std::string("send: ") + std::strerror(errno));

        // Notifier le destinataire que le fichier est prêt
        Client* receiver = findClientByNickname(transfer.receiver_nick);
        if (receiver) {
            std::string file_ready = ":" + _server_name + " NOTICE " + receiver->get_nickname() +
                                    " :Le fichier " + transfer.filename + " de " + transfer.sender_nick +
                                    " est prêt. Utilisez ACCEPT " + transfer_id + " pour le recevoir.\r\n";
            ssize_t sent2 = send(receiver->get_socket(), file_ready.c_str(), file_ready.length(), 0);
            if (sent2 < 0)
                throw std::runtime_error(std::string("send: ") + std::strerror(errno));
        }
    } else {
        // Envoyer une confirmation que le chunk a été reçu
        std::string chunk_received = ":" + _server_name + " NOTICE " + client->get_nickname() +
                                   " :Chunk reçu. " + toString(transfer.data.size()) +
                                   "/" + toString(transfer.filesize) + " octets.\r\n";
        ssize_t sent = send(client_fd, chunk_received.c_str(), chunk_received.length(), 0);
        if (sent < 0)
            throw std::runtime_error(std::string("send: ") + std::strerror(errno));
    }
}

void Server::handleSend(int client_fd, std::vector<std::string> params, Client* client) {
    // SEND target filename filesize
    if (!client->isRegistered()) {
        sendClientError(client_fd, "451", params[0]);
        return;
    }

    if (params.size() != 4) {
        if (params.size() < 4)
            sendClientError(client_fd, "461", params[0]);
        else if (params.size() > 4)
            sendClientError(client_fd, "459", params[0]);
        return;
    }

    ssize_t filesize;
    try {
        filesize = std::atol(params[3].c_str());
        if (filesize < 0)
            throw std::exception();
    }
    catch (...) {
        sendClientError(client_fd, "602", params[0]);
        return;
    }

    std::string nick = params[1];
    Client* target = NULL;
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->get_nickname() == nick) {
            target = *it;
            break;
        }
    }

    if (!target) {
        sendClientError(client_fd, "401", nick);
        return;
    }

    std::string filename = params[2];

    std::ostringstream oss;
    oss << time(NULL);

    std::string temps = oss.str();
    std::string transfer_id = client->get_nickname() + "_" + nick + "_" + filename + "_" + temps;

    // Créer un nouveau transfert de fichier
    FileTransfer transfer;
    transfer.transfer_id = transfer_id;
    transfer.sender_nick = client->get_nickname();
    transfer.receiver_nick = nick;
    transfer.filename = filename;
    transfer.filesize = filesize;
    transfer.transfer_complete = false;

    // Ajouter un timestamp pour l'expiration
    transfer.timestamp = time(NULL);

    _fileTransfers[transfer_id] = transfer;

    // Notifier l'émetteur
    std::string upload_instructions = ":" + _server_name + " NOTICE " + client->get_nickname() +
                                    " :Utilisez la commande UPLOAD " + transfer_id + " [data] pour envoyer les données du fichier.\r\n";
    ssize_t sent = send(client_fd, upload_instructions.c_str(), upload_instructions.length(), 0);
    if (sent < 0)
        throw std::runtime_error(std::string("send: ") + std::strerror(errno));

    // Notifier le destinataire
    std::string file_pending = ":" + _server_name + " NOTICE " + nick +
                             " :" + client->get_nickname() + " veut vous envoyer le fichier " + filename +
                             " (" + toString(filesize) + " octets). ID de transfert: " + transfer_id + ".\r\n";
    sent = send(target->get_socket(), file_pending.c_str(), file_pending.length(), 0);
    if (sent < 0)
        throw std::runtime_error(std::string("send: ") + std::strerror(errno));

    std::string info_msg = ":" + _server_name + " NOTICE " + nick + " :Utilisez ACCEPT " + transfer_id +
                         " pour accepter ou DECLINE " + transfer_id + " pour refuser le transfert.\r\n";
    sent = send(target->get_socket(), info_msg.c_str(), info_msg.length(), 0);
    if (sent < 0)
        throw std::runtime_error(std::string("send: ") + std::strerror(errno));
}

void Server::handleAccept(int client_fd, std::vector<std::string> params, Client* client) {
    if (params.size() != 2) {
        sendClientError(client_fd, "461", params[0]);
        return;
    }

    std::string transfer_id = params[1];

    // Vérifier si c'est un transfert de fichier
    std::map<std::string, FileTransfer>::iterator ft_it = _fileTransfers.find(transfer_id);
    if (ft_it != _fileTransfers.end()) {
        FileTransfer& transfer = ft_it->second;

        // Vérifier que le client est bien le destinataire
        if (transfer.receiver_nick != client->get_nickname()) {
            sendClientError(client_fd, "603", params[0]);
            return;
        }

        // Vérifier que le fichier a été complètement uploadé
        if (!transfer.transfer_complete) {
            std::string wait_msg = ":" + _server_name + " NOTICE " + client->get_nickname() +
                                 " :Le fichier n'a pas encore été complètement uploadé. Veuillez attendre.\r\n";
            ssize_t sent = send(client_fd, wait_msg.c_str(), wait_msg.length(), 0);
            if (sent < 0)
                throw std::runtime_error(std::string("send: ") + std::strerror(errno));
            return;
        }

        // Envoyer le fichier au destinataire
        std::string start_msg = ":" + _server_name + " NOTICE " + client->get_nickname() +
                              " :Début du transfert de " + transfer.filename + ".\r\n";
        ssize_t sent = send(client_fd, start_msg.c_str(), start_msg.length(), 0);
        if (sent < 0)
            throw std::runtime_error(std::string("send: ") + std::strerror(errno));

        // Envoyer le fichier en morceaux
        size_t chunk_size = 4096;
        for (size_t i = 0; i < transfer.data.size(); i += chunk_size) {
            size_t current_chunk_size = std::min(chunk_size, transfer.data.size() - i);

            // Envoyer un en-tête pour le morceau
            std::string header = ":" + _server_name + " FILEDATA " + client->get_nickname() +
                               " " + transfer_id + " " + toString(i) + " " +
                               toString(current_chunk_size) + "\r\n";
            ssize_t sent_header = send(client_fd, header.c_str(), header.length(), 0);
            if (sent_header < 0)
                throw std::runtime_error(std::string("send header: ") + std::strerror(errno));

            // Envoyer les données
            ssize_t sent_data = send(client_fd, &transfer.data[i], current_chunk_size, 0);
            if (sent_data < 0)
                throw std::runtime_error(std::string("send data: ") + std::strerror(errno));

            // Envoyer la progression toutes les 5 chunks
            if ((i / chunk_size) % 5 == 0 || i + chunk_size >= transfer.data.size()) {
                int progress = (int)((i + current_chunk_size) * 100 / transfer.data.size());
                std::string progress_msg = ":" + _server_name + " NOTICE " + client->get_nickname() +
                                        " :Transfert de " + transfer.filename + ": " +
                                        toString(progress) + "% terminé.\r\n";
                ssize_t sent_progress = send(client_fd, progress_msg.c_str(), progress_msg.length(), 0);
                if (sent_progress < 0)
                    throw std::runtime_error(std::string("send progress: ") + std::strerror(errno));
            }
        }

        // Notifier que le transfert est terminé
        std::string complete_msg = ":" + _server_name + " NOTICE " + client->get_nickname() +
                                 " :Transfert de " + transfer.filename + " terminé avec succès.\r\n";
        sent = send(client_fd, complete_msg.c_str(), complete_msg.length(), 0);
        if (sent < 0)
            throw std::runtime_error(std::string("send: ") + std::strerror(errno));

        // Notifier l'émetteur
        Client* sender = findClientByNickname(transfer.sender_nick);
        if (sender) {
            std::string sender_msg = ":" + _server_name + " NOTICE " + sender->get_nickname() +
                                   " :" + client->get_nickname() + " a reçu avec succès le fichier " +
                                   transfer.filename + ".\r\n";
            ssize_t sent2 = send(sender->get_socket(), sender_msg.c_str(), sender_msg.length(), 0);
            if (sent2 < 0)
                throw std::runtime_error(std::string("send: ") + std::strerror(errno));
        }

        // Supprimer le transfert
        _fileTransfers.erase(transfer_id);
        return;
    }

    // Si ce n'est pas un transfert de fichier, utiliser l'ancien code pour les transferts DCC
    // [Le reste de votre code handleAccept original]
}

void Server::handleDecline(int client_fd, std::vector<std::string> params, Client* client){
    if (params.size() != 2) {
        sendClientError(client_fd, "461", params[0]);
        return;
    }

    std::string transfer_id = params[1];

    std::map<std::string, FileTransfer>::iterator it = _fileTransfers.find(transfer_id);
    if (it == _fileTransfers.end()){
        sendClientError(client_fd, "604", params[0]);
        return;
    }

    FileTransfer& transfer = it->second;

    if (transfer.receiver_nick != client->get_nickname()){
        sendClientError(client_fd, "603", params[0]);
        return;
    }

    std::string decline_msg = ":" + _server_name + " NOTICE " + client->get_nickname() +
                                " :You refuse the transfert of " + transfer.filename +
                                " from " + transfer.sender_nick + "\r\n";

    ssize_t sent = send(client_fd, decline_msg.c_str(), decline_msg.length(), 0);
    if (sent < 0)
        throw std::runtime_error(std::string("send: ") + std::strerror(errno));

    Client *sender = NULL;
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->get_nickname() == transfer.sender_nick) {
            sender = *it;
            break;
        }
    }

    if (sender) {
        std::string sender_msg = ":" + _server_name + " NOTICE " + sender->get_nickname() +
                                    " :" + client->get_nickname() + " has refuse to receive " +
                                    transfer.filename + "\r\n";

        ssize_t sent2 = send(sender->get_socket(), sender_msg.c_str(), sender_msg.length(), 0);
        if (sent2 < 0)
            throw std::runtime_error(std::string("send: ") + std::strerror(errno));
    }

    _fileTransfers.erase(transfer_id);
}

void Server::handleSocketActivity() {
    fd_set read_fds = _master_read_fds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    if (select(_max_fd + 1, &read_fds, NULL, NULL, &tv) == -1)
        throw std::runtime_error(std::string("select: ") + std::strerror(errno));


    // Parcourir tous les descripteurs de fichiers
    for (int i = 0; i <= _max_fd; i++) {
        if (FD_ISSET(i, &read_fds)) {
            // Vérifier si c'est un transfert DCC actif
            std::map<int, TransferSession>::iterator it = _activeTransfers.find(i);
            if (it != _activeTransfers.end()) {
                handleTransferActivity(i, it->second);
            }
            // Sinon c'est géré ailleurs dans votre code principal du serveur
        }
    }
}

void Server::handleTransferActivity(int sock_fd, TransferSession& session) {
    char buffer[8192];
    ssize_t bytes_read = recv(sock_fd, buffer, sizeof(buffer), 0);

    if (bytes_read > 0) {
        // Mise à jour des statistiques de transfert
        session.bytes_transferred += bytes_read;
        session.last_activity = time(NULL);

        // Calculer le pourcentage de progression
        int progress = (int)((session.bytes_transferred * 100) / session.filesize);

        // Informer périodiquement les clients (par exemple, tous les 10%)
        if (progress % 10 == 0 && session.last_progress_report != progress) {
            session.last_progress_report = progress;

            // Trouver les clients pour envoyer les notifications
            Client* sender = findClientByNickname(session.sender_nick);
            Client* receiver = findClientByNickname(session.receiver_nick);

            std::string prog = toString(progress);
            std::string bytes_transfert = toString(session.bytes_transferred);
            std::string f_size = toString(session.filesize);

            std::string progress_msg = ":" + _server_name + " NOTICE %s" + " :Transfert de " + session.filename + ": " +
                                    prog + "%% terminé (" + bytes_transfert + "/" + f_size + " octets)\r\n";

            if (sender) {
                std::string sender_msg = progress_msg;
                sender_msg.replace(sender_msg.find("%s"), 2, sender->get_nickname());
                ssize_t sent = send(sender->get_socket(), sender_msg.c_str(), sender_msg.length(), 0);
                if (sent < 0)
                    throw std::runtime_error(std::string("send: ") + std::strerror(errno));
            }

            if (receiver) {
                std::string receiver_msg = progress_msg;
                receiver_msg.replace(receiver_msg.find("%s"), 2, receiver->get_nickname());
                ssize_t sent2 = send(receiver->get_socket(), receiver_msg.c_str(), receiver_msg.length(), 0);
                if (sent2 < 0)
                    throw std::runtime_error(std::string("send: ") + std::strerror(errno));
            }
        }

        // Si le transfert est terminé
        if (session.bytes_transferred >= session.filesize) {
            finishTransfer(sock_fd, session, true);
        }
    }

    else if (bytes_read == 0) {
        // Connexion fermée par l'autre côté
        finishTransfer(sock_fd, session, session.bytes_transferred >= session.filesize);
    }
    else if (bytes_read == -1) {
        // Erreur ou non-bloquant sans données à lire
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::string error = std::strerror(errno);
            finishTransfer(sock_fd, session, false, "Error while reading: " + error);
        }
    }
}

void Server::finishTransfer(int sock_fd, TransferSession& session, bool success, std::string error_msg) {
    // Trouver les clients pour les notifications
    Client* sender = findClientByNickname(session.sender_nick);
    Client* receiver = findClientByNickname(session.receiver_nick);

    std::string status_msg;
    if (success) {
        status_msg = ":" + _server_name + " NOTICE %s" +
                   " :Transfert de " + session.filename + " terminé avec succès.\r\n";
    } else {
        status_msg = ":" + _server_name + " NOTICE %s" +
                   " :Transfert de " + session.filename + " échoué. " + error_msg + "\r\n";
    }

    if (sender) {
        std::string sender_msg = status_msg;
        sender_msg.replace(sender_msg.find("%s"), 2, sender->get_nickname());
        ssize_t sent = send(sender->get_socket(), sender_msg.c_str(), sender_msg.length(), 0);
        if (sent < 0)
            throw std::runtime_error(std::string("send: ") + std::strerror(errno));
    }

    if (receiver) {
        std::string receiver_msg = status_msg;
        receiver_msg.replace(receiver_msg.find("%s"), 2, receiver->get_nickname());
        ssize_t sent2 = send(receiver->get_socket(), receiver_msg.c_str(), receiver_msg.length(), 0);
        if (sent2 < 0)
            throw std::runtime_error(std::string("send: ") + std::strerror(errno));
    }

    // Nettoyer les ressources
    FD_CLR(sock_fd, &_master_read_fds);
    safeClose(sock_fd);
    _activeTransfers.erase(sock_fd);

    // Recalculer _max_fd si nécessaire
    if (sock_fd == _max_fd) {
        _max_fd = 0;
        for (int i = 0; i <= sock_fd; i++) {
            if (FD_ISSET(i, &_master_read_fds) && i > _max_fd) {
                _max_fd = i;
            }
        }
    }
}

void Server::cleanupExpiredTransfers() {
    time_t current_time = time(NULL);

    // Nettoyer les transferts en attente expirés (plus de 5 minutes)
    std::vector<std::string> expired_transfers;
    for (std::map<std::string, FileTransfer>::iterator it = _fileTransfers.begin();
         it != _fileTransfers.end(); ++it) {
        if (current_time - it->second.timestamp > 300) { // 5 minutes
            expired_transfers.push_back(it->first);
        }
    }

    for (std::vector<std::string>::iterator it = expired_transfers.begin();
         it != expired_transfers.end(); ++it) {
            FileTransfer& transfer = _fileTransfers[*it];

        // Notifier les clients
        Client* sender = findClientByNickname(transfer.sender_nick);
        Client* receiver = findClientByNickname(transfer.receiver_nick);

        if (sender) {
            std::string msg = ":" + _server_name + " NOTICE " + sender->get_nickname() +
                            " :Le transfert de " + transfer.filename + " vers " +
                            transfer.receiver_nick + " a expiré.\r\n";
            ssize_t sent = send(sender->get_socket(), msg.c_str(), msg.length(), 0);
            if (sent < 0)
                throw std::runtime_error(std::string("send: ") + std::strerror(errno));
        }

        if (receiver) {
            std::string msg = ":" + _server_name + " NOTICE " + receiver->get_nickname() +
                            " :Le transfert de " + transfer.filename + " depuis " +
                            transfer.sender_nick + " a expiré.\r\n";
            ssize_t sent2 = send(receiver->get_socket(), msg.c_str(), msg.length(), 0);
            if (sent2 < 0)
                throw std::runtime_error(std::string("send: ") + std::strerror(errno));
        }

        _fileTransfers.erase(*it);
    }

    // Nettoyer les transferts actifs inactifs (plus de 2 minutes)
    std::vector<int> inactive_transfers;
    for (std::map<int, TransferSession>::iterator it = _activeTransfers.begin();
         it != _activeTransfers.end(); ++it) {
        if (current_time - it->second.last_activity > 120) { // 2 minutes
            inactive_transfers.push_back(it->first);
        }
    }

    for (std::vector<int>::iterator it = inactive_transfers.begin();
         it != inactive_transfers.end(); ++it) {
        finishTransfer(*it, _activeTransfers[*it], false, "Timeout de connexion");
    }
}

void Server::handleKick(int client_fd, std::vector<std::string> params, Client* client){
	if (!client->isRegistered()) {
		sendClientError(client_fd, "451", params[0]);
		return;
	}

	if (params.size() < 3 || params.size() > 4 ){
		if (params.size() < 3)
			sendClientError(client_fd, "461", params[0]);
		if (params.size() > 4)
			sendClientError(client_fd, "459", params[0]);
		return;
	}

	int res = client->execute_command(params, _clients, _channels);
	if (res != 0){
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
	}
}

void Server::handleUser(int client_fd, std::vector<std::string> params, Client* client){
	if (!client->get_bool_pass()) {
		sendClientError(client_fd, "464", params[0]);
		return;
	}

	if (params.size() != 5){
		sendClientError(client_fd, "461", params[0]);
		return;
	}

	if (!isValidUsername(params[1])){
		sendClientError(client_fd, "430", params[0]);
		return;
	}

	int res = client->execute_command(params, _clients, _channels);
	if (res != 0){
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
		return;
	}

	client->set_bool_user(true);
	if (client->isRegistered() && client->get_autentification() == false)
		sendWelcome(client_fd, client);
}

void Server::handleNick(int client_fd, std::vector<std::string> params, Client* client){
	if (!client->get_bool_pass()) {
		sendClientError(client_fd, "464", params[0]);
		return;
	}

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
	if (res != 0){
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
		return;
	}

	client->set_bool_nick(true);
	if (client->isRegistered() && client->get_autentification() == false)
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
	if (!client->isRegistered()) {
		sendClientError(client_fd, "451", params[0]);
		return;
	}

	if (params.size() != 3){
		sendClientError(client_fd, "461", params[0]);
		return;
	}

	if (!isValidChannel(params[2])){
		sendClientError(client_fd, "476", params[1]);
		return;
	}

	int res = client->execute_command(params, _clients, _channels);
	if (res != 0){
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
		return;
	}
}

void Server::handleTopic(int client_fd, std::vector<std::string> params, Client* client){
	if (!client->isRegistered()) {
		sendClientError(client_fd, "451", params[0]);
		return;
	}

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
	if (res != 0){
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
		return;
	}
}

void Server::handleJoin(int client_fd, std::vector<std::string> params, Client* client){
	if (!client->isRegistered()) {
		sendClientError(client_fd, "451", params[0]);
		return;
	}

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
		if (!isNotTakenChannel(new_channel_name)){
			sendClientError(client_fd, "434", params[0]);
			return;
		}
		Channel* new_channel = new  Channel(new_channel_name, client);
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
	else if (res != 0){
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
	}
}

void Server::handlePrivmsg(int client_fd, std::vector<std::string> params, Client* client){
	if (!client->isRegistered()) {
		sendClientError(client_fd, "451", params[0]);
		return;
	}

	if (params.size() != 3){
		sendClientError(client_fd, "411", params[0]);
		return;
	}

	if (params[2][0] == '!'){
		std::transform(params[2].begin(), params[2].end(), params[2].begin(), ::toupper);
		handleCommandBotPriv(client_fd, params, client);
		return;
	}

	int res = client->execute_command(params, _clients, _channels);// ereur a check
	if (res != 0 )
	{
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
	}
}

void Server::handleMode(int client_fd, std::vector<std::string> params, Client* client){
	if (!client->isRegistered()) {
		sendClientError(client_fd, "451", params[0]);
		return;
	}

	if (params.size() < 2){
		sendClientError(client_fd, "461", params[0]);
		return;
	}

	if (!isValidChannel(params[1])){
		sendClientError(client_fd, "476", params[1]);
		return;
	}

	int res = client->execute_command(params, _clients, _channels);
	if (res != 0 )
	{
		std::ostringstream oss;
		oss << res;
		sendClientError(client_fd, oss.str() ,params[0]);
	}
	// Si tentative de modifier le mode d'un autre utilisateur : 502
}

void Server::handlePass(int client_fd, std::vector<std::string> params, Client* client){
	if (params.size() != 2){
		sendClientError(client_fd, "461", params[0]);
		return;
	}

	if (client->get_bool_pass()){
		sendClientError(client_fd, "462", params[0]);
		return;
	}

	if (params[1] != _password){
		sendClientError(client_fd, "464", params[0]);
		return;
	}

	client->set_bool_pass(true);
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

	// Initialisation des ensembles de descripteurs de fichiers pour select()
	FD_ZERO(&_master_read_fds);
	FD_SET(_server_socket, &_master_read_fds);
	_max_fd = _server_socket;

	time_t last_cleanup = time(NULL);

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
		handleSocketActivity();
		time_t now = time(NULL);
		if (now - last_cleanup > 60) {
			cleanupExpiredTransfers();
			last_cleanup = now;
		}
	}
	cleanup();
}

Server::Server(int port, std::string password) : _port(port), _password(password), _server_name("IRC"){
	_isON = 1;
	FD_ZERO(&_master_read_fds);
	_max_fd = 0;

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