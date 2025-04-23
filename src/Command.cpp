/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 13:07:04 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/22 15:26:23 by dsindres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Command.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"

Command::Command(){};

Command::~Command(){};

int Command::kick(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels)
{
    if (this->verif_client(input[2], clients) == 1)
    {
        std::cout << "This client doesn't exist" << std::endl;
        return (1);
    }
    std::vector<Channel*>::iterator it = channels.begin();
    std::string channel_name = input[1];
    channel_name.erase(0,1);
    while(it != channels.end())
    {
        if ((*it)->get_name() == channel_name)
        {
            Client *test = (*it)->get_client(input[2]);
            if (test != NULL)
            {
                // (*it)->remove_client(test);
                // test->_channels.erase(it);
                test->leave_channel(channel_name, channels);
                if (input.size() > 3)
                    std::cout << input[2] << " is ejected from " << channel_name << " channel because " << input[3] << std::endl;
                else
                    std::cout << input[2] << " is ejected from " << channel_name << " channel" << std::endl;
                return (0);
            }
            // ERROR
            std::cout << "This client is not in this channel" << std::endl;
            return (1);
        }
        it++;
    }
    return (1); 
}


int Command::send_message(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels)
{
    if (input[1][0] == '#')
    {
        std::string channel_name = input[1];
        channel_name.erase(0,1);
        std::vector<Channel*>::iterator it = channels.begin();
        while (it != channels.end())
        {
            if ((*it)->get_name() == channel_name)
            {
                (*it)->send_message(input[2]);
                return (1);
            }
            it++;
        }
        return (1);
    }
    else if (this->verif_client(input[1], clients) == 0)
    {
        std::vector<Client*>::iterator it = clients.begin();
        while (it != clients.end())
        {
            if ((*it)->get_nickname() == input[1] || (*it)->get_username() == input[1])
            {
                std::string msg = input[2] + "\r\n";
                ssize_t bytes_sent = send((*it)->get_socket(), msg.c_str(), msg.length(), 0);
                if (bytes_sent < 0)
                {
                    std::cerr << "Error : " << (*it)->get_nickname() << " doesn't receive the message" << std::endl;
                    return (1);
                }
                return (0); 
            }
            it++;
        }
    }
    std::cerr << "The client " << input[1] << " doesn't exist" << std::endl;
    return (401);
}


int Command::verif_client(std::string client_to_verif, std::vector<Client*> clients)
{
    std::vector<Client*>::iterator it = clients.begin();
    while(it != clients.end())
    {
        if ((*it)->get_nickname() == client_to_verif || (*it)->get_username() == client_to_verif)
        {
            return (0);
        }
        it++;
    }
    return (1);
}

int Command::verif_channel(std::string channel_to_verif, std::vector<Channel*> channels)
{
    std::vector<Channel*>::iterator it = channels.begin();
    while(it != channels.end())
    {
        if ((*it)->get_name() == channel_to_verif)
        {
            return (0);
        }
        it++;
    }
    return (1);
}