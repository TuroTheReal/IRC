/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:26:09 by dsindres          #+#    #+#             */
/*   Updated: 2025/05/05 11:41:50 by dsindres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <memory>
#include <list>
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>



class Client;
class Channel;

class Command
{
    public:
        Command();
        ~Command();

        // COMMANDES
        int kick(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client);
        int send_message(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client);
        int invite(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client);
        int topic(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client);
        int mode(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client);

        
        // Fonctions utiles
        int verif_client(std::string client_to_verif, std::vector<Client*> clients);
        int verif_channel(std::string channel_to_verif, std::vector<Channel*> channels);
        int is_number(std::string nbr);
        int is_valid_password(std::string pass);
        std::string is_valid_client(std::vector<std::string> input, int index, std::vector<Client*> clients, Channel *channel);
        std::string is_valid_client_2(std::vector<std::string> input, int index, std::vector<Client*> clients, Channel *channel);

    private:
};