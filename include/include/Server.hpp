/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:24:38 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/16 14:20:17 by dsindres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sys/socket.h>
#include <poll.h>
#include "Client.hpp"

class Server 
{
    public:
        Server(int port, std::string password);
        void start();
        // void handle_new_connection();
        // void handle_client_message(std::shared_ptr<Client> client);

        class EmptyPasswordException : public std::exception
        {
            public :
                const char* what() const throw();
        };
        class BadPortException : public std::exception
        {
            public :
                const char* what() const throw();
        };

    private:
        int _port;
        std::string _password;
        int _server_fd;
        std::vector<Client*> _clients;
        std::vector<Channel*> _channels;
};