/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:25:46 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/23 12:24:10 by dsindres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>
#include <memory>

class Client;

class Channel
{
    public:
        Channel();
        Channel(std::string name, Client *opera);
        ~Channel();
        
        // Get attributs
        std::string get_name();
        Client *get_operator();
        Client *get_client(std::string client_name);
        std::string get_pass();
        void set_operator();
        int get_limit();
        bool get_on_invit();

        //DEBUG
        void get_all_clients();
        
        // Statut du Channel
        void  status_channel();
        
        void add_client(Client *client);
        void remove_client(Client *client);
        void send_message(const std::string &message);
        int get_nbr_of_client();

    private:
        std::string _name;
        std::string _password;
        int _limit;
        bool _on_invit;
        std::vector<Client*> _clients;
        Client *opera;
};