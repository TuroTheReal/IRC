/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:25:10 by dsindres          #+#    #+#             */
/*   Updated: 2025/05/14 11:00:34 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstring>
#include <string>
#include <memory>
#include <list>
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>


class Command;
class Channel;
//class Server;

class Client
{
    public:
        Client();
        Client(int socket);
        ~Client();

        // Set attributs
        int set_nickname(std::string nickname, std::vector<Client*> clients, std::vector<Channel*>channels);
        int set_username(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels);
        void set_operator(bool is_operator);
        void set_autentification(bool reponse);
		void set_bool_nick(bool response);
		void set_bool_user(bool response);
		void set_bool_pass(bool response);

        // Get attributs
        std::string get_nickname();
        std::string get_username();
        int get_socket();
        int is_operator(std::string channel_name);
        bool get_autentification();
		bool get_bool_pass();
		bool get_bool_nick();
		bool get_bool_user();
		bool isRegistered();

		void set_irrsi(bool);
		bool get_irrsi();


        // Gestion des channels
        int join_channel(std::vector<std::string> input, std::vector<Channel*> &channels);
        void leave_channel(std::string channel_name, std::vector<Channel*> &channels);
        bool is_in_channel(std::string channel_name);
        void leave_channel_from_dest_channel(Channel *channel);

        // Commandes
        int execute_command(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels);
        int privmsg(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels);
        int kick(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels);
        int invite(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels);
        int topic(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels);
        int mode(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels);

        // Fonctions utiles
        void receive_message(const std::string &message, int socket);
        bool get_invited_by(Channel *channel);
        void add_channel_operator(Channel *channel);
        void supp_channel_operator(Channel *channel);
        void add_channel_invited(Channel *channel);
        void send_message(std::string message, std::vector<Client*> clients, std::vector<Channel*>channels);
        void join_message(Channel *channel);
        std::vector<Channel*>  supp_channel();
        int operator_vector_nul();

        //DEBUG
        void get_operator();
        void get_channel();
        void get_invitation();
        void XX();
        void XXX(std::vector<std::string> input, std::vector<Channel*>channels);


    private:
        int _socket;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        bool _is_authenticated;
        bool _is_operator;
		bool _hasPassword;
		bool _hasNick;
		bool _hasUser;
		bool _irssi;
        std::vector<Channel*> _invited_channels;
        std::vector<Channel*> _channels;
        std::vector<Channel*> _operator_channels;
        Command *_command;
};