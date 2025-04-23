/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 15:25:11 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/23 16:25:00 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Command.hpp"

Channel::Channel(){};

Channel::Channel(std::string name, Client *opera)
{
    this->_name = name;
    this->opera = opera;
    this->_password = "";
    this->_on_invit = false;
    this->_limit = -1;
}

Channel::~Channel()
{
    std::vector<Client*>::iterator it = this->_clients.begin();
    while (it != this->_clients.end())
    {
        if (*it)
            (*it)->leave_channel_from_dest_channel(this);
        it++;
    }
    this->_clients.clear();
}

//----------------------------- GETTER et SETTER ------------------------------------

std::string Channel::get_name()
{
    return (this->_name);
}

Client *Channel::get_operator()
{
    return (this->opera);
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

void Channel::set_operator()
{
    this->opera = NULL;
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
        (*it)->receive_message(message);
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

//----------------------------- DEBUG ------------------------------------

void Channel::get_all_clients()
{
    std::vector<Client*>::iterator it = this->_clients.begin();
    while(it != _clients.end())
    {
        std::cout << "                 " << (*it)->get_username() << std::endl;
        it++;
    }
}

// COMMENT TU GERES SI LE CLIENT QUITTE< TU LAISSE DANS LE CHANNEL OU PAS ?