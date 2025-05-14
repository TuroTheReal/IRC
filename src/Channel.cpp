/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 15:25:11 by dsindres          #+#    #+#             */
/*   Updated: 2025/05/12 17:27:37 by dsindres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Command.hpp"
#include <algorithm>

Channel::Channel(){};

Channel::Channel(std::string name, Client *opera)
{
    this->_name = name;
    //this->opera = opera;
    this->_topic = "default";
    this->_password = "";
    this->_on_invit = false;
    this->_is_restriction_topic = false;
    this->_limit = -1;
    this->_operator_clients.push_back(opera);
    this->_creationTime = std::time(NULL);

}

Channel::~Channel()
{
    // std::vector<Client*>::iterator it = this->_clients.begin();
    // while (it != this->_clients.end())
    // {
    //     if (*it)
    //         (*it)->leave_channel_from_dest_channel(this);
    //     it++;
    // }
    this->_operator_clients.clear();
    this->_clients.clear();
}

//----------------------------- GETTER et SETTER ------------------------------------

std::string Channel::get_name()
{
    return (this->_name);
}

bool Channel::get_operator_bool()
{
    if (this->_operator_clients.empty())
        return (false);
    return (true);
}

time_t Channel::getCreationTime() 
{
    return this->_creationTime;
}

Client *Channel::get_operator(Client *client)
{
    std::vector<Client*>::iterator it = _operator_clients.begin();
    while (it != _operator_clients.end())
    {
        if(client == (*it))
            return (*it);
        it++;
    }
    return (NULL);
}

std::string Channel::get_pass()
{
    return (this->_password);
}

int Channel::get_limit()
{
    return (this->_limit);
}

bool Channel::get_on_invit()
{
    return (this->_on_invit);
}

bool Channel::get_restriction_topic()
{
    return (this->_is_restriction_topic);
}

Client *Channel::get_client(std::string client_name)
{
    std::vector<Client*>::iterator it = this->_clients.begin();
    while (it != this->_clients.end())
    {
        if ((*it)->get_nickname() == client_name || (*it)->get_username() == client_name)
        {
            return (*it);
        }
        it++;
    }
    return NULL;
}

void Channel::set_operator(Client* client)
{
    std::vector<Client*>::iterator it = _operator_clients.begin();
    while(it != _operator_clients.end())
    {
        if (*it == client)
        {
            _operator_clients.erase(it);
            return ;
        }
        it++;
    }
}

void Channel::set_pass(std::string pass)
{
    this->_password = pass;
}

void Channel::set_limit(int limit)
{
    this->_limit= limit;
}

void Channel::set_restriction_topic(bool response)
{
    this->_is_restriction_topic = response;
}

std::string Channel::get_topic()
{
    return (this->_topic);
}

void Channel::set_on_invit(bool reponse)
{
    this->_on_invit = reponse;
}

void Channel::set_topic(std::string topic_message)
{
    this->_topic = topic_message;
}

//----------------------------- METHODES ------------------------------------

void    Channel::status_channel()
{
    std::vector<Client*>::iterator it = _clients.begin();
    std::cout << " In this channel : " << this->_name << std::endl;
    while(it != _clients.end())
    {
        std::cout << "client = " << (*it)->get_nickname() << std::endl;
        it++;
    }
}

void    Channel::add_client(Client *client)
{
    this->_clients.push_back(client);
}

void    Channel::remove_client(Client *client)
{
    std::vector<Client*>::iterator it = _clients.begin();
    while(it != _clients.end())
    {
        if (*it == client)
        {
            _clients.erase(it);
            std::cout << "Client " << client->get_nickname() << " removed from channel " << this->_name << std::endl;
            return ;
        }
        it++;
    }
}

void    Channel::send_message(std::string const &message)
{
    std::vector<Client*>:: iterator it = _clients.begin();
    while (it != _clients.end())
    {
        (*it)->receive_message(message, (*it)->get_socket());
        it++;
    }
}

void    Channel::send_message_except(std::string const &message, Client *client)
{
    std::vector<Client*>:: iterator it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it) != client)
            (*it)->receive_message(message, (*it)->get_socket());
        it++;
    }
}

int Channel::get_nbr_of_client()
{
    std::vector<Client*>:: iterator it = _clients.begin();
    int res = 0;
    while (it != _clients.end())
    {
        res++;
        it++;
    }
    return (res);
}

void    Channel::add_operator(Client *client)
{
    this->_operator_clients.push_back(client);
}

void    Channel::supp_operator(Client *client)
{
    std::vector<Client*>::iterator it = this->_operator_clients.begin();
    while (it != this->_operator_clients.end())
    {
        if ((*it) == client)
        {
            this->_operator_clients.erase(it);
            return ;
        }
        it++;
    }
}

std::string Channel::join_message()
{
    std::vector<Client*>::iterator it = this->_clients.begin();
    std::string mess = "";
    while (it != this->_clients.end())
    {
        mess = mess + " " + (*it)->get_nickname();
        it++;
    }
    return (mess);
}

//----------------------------- DEBUG ------------------------------------

void Channel::get_all_clients()
{
    std::string sp = "             ";
    std::vector<Client*>::iterator it = this->_clients.begin();
    while(it != _clients.end())
    {
        std::cout << "    client = " << (*it)->get_nickname() << std::endl;
        it++;
    }
    std::vector<Client*>::iterator ite = this->_operator_clients.begin();
    while(ite != _operator_clients.end())
    {
        std::cout << "       ope = " << (*ite)->get_nickname() << std::endl;
        ite++;
    }
}
