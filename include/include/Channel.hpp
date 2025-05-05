/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:25:46 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/30 15:04:12 by dsindres         ###   ########.fr       */
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
        bool get_operator_bool();
        Client *get_operator(Client *client);
        Client *get_client(std::string client_name);
        std::string get_pass();
        void set_operator(Client *client);
        int get_limit();
        bool get_on_invit();
        void set_on_invit(bool reponse);
        bool get_restriction_topic();
        void set_restriction_topic(bool response);
        std::string get_topic();
        void set_topic(std::string topic_message);
        void set_pass(std::string pass);
        void set_limit(int limit);

        //DEBUG
        void get_all_clients();
        
        // Statut du Channel
        void  status_channel();
        
        void add_client(Client *client);
        void remove_client(Client *client);
        void send_message(const std::string &message);
        void send_message_except(const std::string &message, Client *client);
        int  get_nbr_of_client();
        void add_operator(Client *client);
        void supp_operator(Client *client);
        std::string join_message();

    private:
        std::string _name;
        std::string _password;
        int _limit;
        bool _on_invit;
        bool _is_restriction_topic;
        std::string _topic;
        std::vector<Client*> _clients;
        std::vector<Client*> _operator_clients;
};