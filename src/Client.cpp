/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 13:12:33 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/23 13:06:09 by dsindres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Command.hpp"
#include "../include/Channel.hpp"

Client::Client(){};

Client::Client(int socket) 
{
    this->_socket = socket;
    this->_nickname = "default";
    this->_username = "default";
    this->_realname = "default";
    this->_isAuthenticated = false;
    this->_is_operator = false;
    this->_command = new Command();
    // premiere connexion entre client et serveur a faire dans la classe serveur ou client ?
    //this->connect();
    std::cout << "Client join the connexion !" << std::endl;
}


Client::~Client()
{
    delete this->_command;
    std::vector<Channel*>::iterator ite = this->_operator_channels.begin();
    while (ite != this->_operator_channels.end())
    {
        if (*ite)
            (*ite)->set_operator();
        ite++;
    }
    std::vector<Channel*>::iterator it = this->_channels.begin();
    while (it != this->_channels.end())
    {
        if (*it)
            (*it)->remove_client(this);
        it++;
    }
    this->_channels.clear();
    this->_operator_channels.clear();
}


//----------------------------- GETTER et SETTER ------------------------------------


std::string Client::get_nickname()
{
    return (this->_nickname);
}

std::string Client::get_username()
{
    return (this->_username);
}

int Client::get_socket()
{
    return (this->_socket);
}

int Client::is_operator(std::string channel_name)
{
    std::vector<Channel*>:: iterator it = this->_operator_channels.begin();
    while(it != this->_operator_channels.end())
    {
        if ((*it)->get_name() == channel_name)
        {
            if ((*it)->get_operator() == this)
                return (0);
            return (1);
        }
        it++;
    }
    return (1);
}


int    Client::set_nickname(std::string nickname, std::vector<Client*> clients, std::vector<Channel*> channels)
{
    std::vector<Client*>:: iterator it = clients.begin();
    if (nickname == "default")
    {
        std::cerr << "Error : you can't take this nickname" << std::endl;
        return (1);
    }
    while (it != clients.end())
    {
        if ((*it)->get_nickname() == nickname)
        {
            std::cerr << "Error: nickname already in use" << std::endl;
            return (1);
        }
        it++;
    }
    std::vector<Channel*>::iterator ite = channels.begin();
    while(ite != channels.end())
    {
        if ((*ite)->get_name() == nickname)
        {
            std::cerr << "Error: nickname already in use" << std::endl;
            return (1);
        }
        ite++;
    }
    this->_nickname = nickname;
    return (0);
}

int    Client::set_username(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels)
{
    std::vector<Client*>:: iterator it = clients.begin();
    if (input[1] == "default")
    {
        std::cerr << "Error : you can't take this username" << std::endl;
        return (1);
    }
    while (it != clients.end())
    {
        if ((*it)->get_username() == input[1])
        {
            std::cerr << "Error: username already in use" << std::endl;
            return (1);
        }
        it++;
    }
    std::vector<Channel*>::iterator ite = channels.begin();
    while(ite != channels.end())
    {
        if ((*ite)->get_name() == input[1])
        {
            std::cerr << "Error: username already in use" << std::endl;
            return (1);
        }
        ite++;
    }
    this->_username = input[1];
    this->_realname = input[4];
    return (0);
}

void    Client::set_operator(bool is_operator)
{
    this->_is_operator = is_operator;
}

//----------------------------- FONCTIONS MEMBRES ------------------------------------


// Nickname DOIT etre set-up sinon segfault
int    Client::join_channel(std::vector<std::string> input, std::vector<Channel*> &channels)
{
    std::string channel_name = input[1];
    channel_name.erase(0, 1);
    std::vector<Channel*>:: iterator my_it = this->_channels.begin();
    while(my_it != this->_channels.end())
    {
        if ((*my_it)->get_name() == channel_name)
        {
            std::cout << "Client " << this->_nickname << " already is in " << channel_name << " channel." << std::endl;
            return (443);
        }
        my_it++;
    }
    std::vector<Channel*>:: iterator it = channels.begin();
    while(it != channels.end())
    {
        if (channel_name == (*it)->get_name())
        {
            if (input.size() > 2)
            {
                if ((*it)->get_on_invit() == true && this->get_invited_by(*it) == false)
                {
                    std::cerr << "Error : you haven't been invited" << std::endl;
                    return (473);
                }
                if (input[2] != (*it)->get_pass() && (*it)->get_pass() != "")
                {
                    std::cerr << "Error : it is not the right password" << std::endl;
                    return (475);
                }
                if ((*it)->get_limit() != -1 && (*it)->get_nbr_of_client() >= (*it)->get_limit())
                {
                    std::cerr << "Channel " << channel_name << " is full" << std::endl;
                    return (471);
                }
                (*it)->add_client(this);
                this->_channels.push_back(*it);
                std::cout << "Client " << this->_nickname << " add in " << (*it)->get_name() << " channel." << std::endl;
                return 0;
            }
            else
            {
                if ((*it)->get_on_invit() == true && this->get_invited_by(*it) == false)
                {
                    std::cerr << "Error : you haven't been invited" << std::endl;
                    return (473);
                }
                if ((*it)->get_pass() != "")
                {
                    std::cerr << "Error : you need a password" << std::endl;
                    return (475);
                }
                if ((*it)->get_limit() != -1 && (*it)->get_nbr_of_client() >= (*it)->get_limit())
                {
                    std::cerr << "Channel " << channel_name << " is full" << std::endl;
                    return (471);
                }
                (*it)->add_client(this);
                this->_channels.push_back(*it);
                std::cout << "Client " << this->_nickname << " add in " << (*it)->get_name() << " channel." << std::endl;
                return 0;
            }
        }
        it++;
    }
    return (11);
}

void    Client::leave_channel(std::string channel_name, std::vector<Channel*> &channels)
{
    std::vector<Channel*>:: iterator it = channels.begin();
    while(it != channels.end())
    {
        if ((*it)->get_name() == channel_name)
        {
            (*it)->remove_client(this);
            break ;
        }
        it++;
    }
    this->_channels.erase(it);
    std::cout << "Client " << this->_nickname << " has left " << channel_name << " channel." << std::endl;
    return ;
}

void    Client::leave_channel_from_dest_channel(Channel *channel)
{
    std::vector<Channel*>::iterator it = this->_channels.begin();
    while (it != this->_channels.end())
    {
        if (*it == channel)
        {
            this->_channels.erase(it);
            return ;
        }
        it++;
    }
    // OU juste 
    //this->_channels.erase(channel);
}

bool    Client::is_in_channel(std::string channel_name)
{
    std::vector<Channel*>::iterator it = _channels.begin();
    while(it != this->_channels.end())
    {
        if (channel_name == (*it)->get_name())
            return true;
        it++;
    }
    return false;
}

void    Client::receive_message(std::string const &message)
{
    std::string msg = message + "\r\n";
    ssize_t bytes_sent = send(this->_socket, msg.c_str(), msg.length(), 0);
    if (bytes_sent < 0)
        std::cerr << "Error : " << this->_nickname << " doesn't send the message" << std::endl;
        
}

int Client::execute_command(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels)
{
    // verifier le leave_channel a la toute fin
    if (input[0] == "KICK") // au moins input.size() > 2
    {
        std::string channel_name = input[1];
        channel_name.erase(0,1);
        if (this->_command->verif_channel(channel_name, channels) == 1)
        {
            std::cerr << "Channel " << channel_name << " doesn't exist" << std::endl;
            return (1);
        }
        if (this->_is_operator == false || this->is_operator(channel_name) == 1)
        {
            std::cerr << "You are not the operator of " << channel_name << " channel" << std::endl;
            return (381);
        }
        int res = this->_command->kick(input, clients, channels);
        return (res);
    }
    if (input[0] == "JOIN")
    {
        int res = this->join_channel(input, channels);
        return (res);
    }
    if (input[0] == "NICK")
    {
       int res = this->set_nickname(input[1], clients, channels);
       return (res);
    }
    if (input[0] == "USER")
    {
       int res = this->set_username(input, clients, channels);
       return (res);
    }
    if (input[0] == "PRIVMSG") // au moins input.size() > 2
    {
        if (input[1][0] == '#')
        {
            std::string channel_name = input[1];
            channel_name.erase(0,1);
            if (this->_command->verif_channel(channel_name, channels) == 0)
            {
                if (this->is_in_channel(channel_name) == false)
                {
                    std::cerr << "The client " << _nickname << " is not in " << channel_name << " channel" << std::endl;
                    return (404);
                }
                int res = this->_command->send_message(input, clients, channels);
                return (res);
            }
            std::cerr << "The channel " << channel_name << " doesn't exist" << std::endl;
            return (1);
        }
        int res = this->_command->send_message(input, clients, channels);
        return (res);
    }
    return (0);
}

bool Client::get_invited_by(Channel *channel)
{
    std::vector<Channel*>::iterator it = _invited_channels.begin();
    while(it != _invited_channels.end())
    {
        if ((*it) == channel)
            return (true);
        it++;
    }
    return (false);
}

void    Client::add_channel_operator(Channel *channel)
{
    this->_operator_channels.push_back(channel);
}


//----------------------------- DEBUG ------------------------------------

void Client::get_operator()
{
    std::vector<Channel*>::iterator it = _operator_channels.begin();
    while(it != _operator_channels.end())
    {
        std::cout << "    operator --> " << (*it)->get_name() << std::endl;
        it++;
    }
}

void Client::get_channel()
{
    std::vector<Channel*>::iterator it = _channels.begin();
    while(it != _channels.end())
    {
        std::cout << "            --> " << (*it)->get_name() << std::endl;
        it++;
    }
}

void Client::get_invitation()
{
    std::vector<Channel*>::iterator it = _channels.begin();
    while(it != _channels.end())
    {
        std::cout << " invitation --> " << (*it)->get_name() << std::endl;
        it++;
    }
}


// void Server::display_all_channel()
// {
//     std::vector<Channel*>::iterator it = _channels.begin();        
//     std::cout << std::endl;
//     while(it != _channel.end())
//     {
//         Client *ope = (*it)->get_operator();
//         std::cout << "  Channel : " << (*it)->get_name() << std::endl;
//         std::cout <<"   operator --> " ope->get_username() << std::endl;
//         (*it)->get_all_clients();
//         std::cout << std::endl;
//         it++;
//     }
//     std::cout << "  -------- end of channels ----------" << std::endl << std::endl;
// }


// void Server::display_all_client()
// {
//     std::vector<Client*>::iterator ite = _clients.begin();
//     std::cout << std::endl;
//     while(ite != _clients.end())
//     {
//         std::cout << "  Client : " << (*it)->get_username() << std::endl;
//         (*ite)->get_operator();
//         (*ite)->get_channel();
//         (*ite)->get_invitation();
//         std::cout << std::endl;
//         ite++;
//     }
//     std::cout << "  -------- end of clients ----------" << std::endl << std::endl;
// }