/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 13:07:04 by dsindres          #+#    #+#             */
/*   Updated: 2025/05/06 13:41:44 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Command.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include <sstream>

Command::Command(){};

Command::~Command(){};

//----------------------------- COMMANDES ------------------------------------


int Command::kick(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client)
{
    if (this->verif_client(input[2], clients) == 1)
    {
        //std::cout << "This client doesn't exist" << std::endl;
        return (401);
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
                if (input.size() > 3)
                {
                    //std::cout << input[2] << " is ejected from " << channel_name << " channel because " << input[3] << std::endl;
                    std::string message;
                    message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost KICK #" + channel_name + " " + test->get_username() + " :" + input[3];
                    (*it)->send_message(message);
                }
                else
                {
                    //std::cout << input[2] << " is ejected from " << channel_name << " channel" << std::endl;
                    std::string message;
                    message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost KICK #" + channel_name + " " + test->get_username();
                    (*it)->send_message(message);
                }
				test->leave_channel(channel_name, channels);
                return (0);
            }
            //std::cout << "This client is not in this channel" << std::endl;
            return (441);
        }
        it++;
    }
    return (0);
}


int Command::send_message(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client)
{
    if (input[1][0] == '#' || input[1][0] == '!' || input[1][0] == '&' || input[1][0] == '+')
    {
        std::string channel_name = input[1];
        channel_name.erase(0,1);
        std::vector<Channel*>::iterator it = channels.begin();
        while (it != channels.end())
        {
            if ((*it)->get_name() == channel_name)
            {
                std::string message;
                message = ":" + client->get_nickname() + "!~" + client->get_username() + "@localhost PRIVMSG #" + channel_name + " :" + input[2];
                (*it)->send_message_except(message, client);
                return (0);
            }
            it++;
        }
        return (0);
    }
    else if (this->verif_client(input[1], clients) == 0)
    {
        std::vector<Client*>::iterator it = clients.begin();
        while (it != clients.end())
        {
            if ((*it)->get_nickname() == input[1])
            {
                std::string message;
                message = ":" + client->get_nickname() + " PRIVMSG " + input[1] + " :" + input[2];
                (*it)->receive_message(message, (*it)->get_socket());
                return (0);
            }
            it++;
        }
    }
    return (401);
}


int Command::invite(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client)
{
    if (this->verif_client(input[1], clients) == 1)
    {
        //std::cout << "This client doesn't exist" << std::endl;
        return (401);
    }
    std::string channel_name = input[2];
    channel_name.erase(0,1);
    std::vector<Channel*>::iterator it = channels.begin();
    while (it != channels.end())
    {
        if (channel_name == (*it)->get_name())
        {
            // if ((*it)->get_on_invit() == false)
            // {
            //     std::cerr << "Error: This channel doesn't need invitation." << std::endl;
            //     return (1);
            // }
            // if ((*it)->get_limit() != -1 && (*it)->get_nbr_of_client() >= (*it)->get_limit())
            // {
            //     std::cerr << "Error: This channel is full." << std::endl;
            //     return (1);
            // }
            break ;
        }
        it++;
    }
    std::vector<Client*>::iterator ite = clients.begin();
    while(ite != clients.end())
    {
        if (input[1] == (*ite)->get_nickname())
        {
            if((*ite)->is_in_channel(channel_name) == false)
            {
                (*ite)->add_channel_invited(*it);
                std::string message;
                message = ":IRC 341 " + client->get_nickname() + " " + (*ite)->get_nickname() + " #" + channel_name;
                client->receive_message(message, client->get_socket());
                std::string message2;
                message = ":" + client->get_nickname() + "!~" + client->get_username() + "@localhost INVITE " + (*ite)->get_nickname() + " :#" + channel_name;
                (*ite)->receive_message(message2, (*ite)->get_socket());
                return (0);
            }
            //std::cerr << "Error : this client is already in this channel" << std::endl;
            return (443);
        }
        ite++;
    }
    return (0);
}

int Command::topic(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client)
{
    (void)clients;
    std::string channel_name = input[1];
    channel_name.erase(0,1);
    std::vector<Channel*>::iterator it = channels.begin();
    while (it != channels.end())
    {
        if (channel_name == (*it)->get_name())
        {
            if (input.size() == 2)
            {
                if ((*it)->get_topic() != "default")
                {
                    //std::cout << "The topic is " << (*it)->get_topic() << std::endl;
                    std::string message;
                    message = ":IRC 332 " + client->get_nickname() + " " + input[1] + " :" + (*it)->get_topic();
                    client->receive_message(message, client->get_socket());
                    return (0); // 332
                }
                //std::cerr << "This channel doesn't have topic yet" << std::endl;
                //std::string message;
                //message = ":IRC 332 " + client->get_nickname() + " " + input[1] + " :No topic is set";
                //client->receive_message(message, client->get_socket());
                return (331);
            }
            else
            {
				if (input[2].empty())
					return 412;
                (*it)->set_topic(input[2]);
                //std::string message = "The topic of " + channel_name + " is " + input[2];
                std::string message;
                message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost TOPIC #" + channel_name + " :" + (*it)->get_topic();
                (*it)->send_message(message);
                return (0);
            }
        }
        it++;
    }
    return (0);
}

int Command::mode(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*>channels, Client *client)
{
    (void)clients;
    std::string channel_name = input[1];
    channel_name.erase(0,1);
    std::vector<Channel*>::iterator it = channels.begin();

    if (input.size() >= 2)
        input.erase(input.begin(), input.begin() + 2);

    if (input[0][0] != '+' && input[0][0] != '-')
    {
        //std::cerr << "Error : bad arguments" << std::endl;
        return (461);
    }
    while (it != channels.end())
    {
        if (channel_name == (*it)->get_name())
        {
            break;
        }
        it++;
    }
	if (verif_mode_char(input) == 1)
		return 501;

    // KOL --> arguments
    // IT  --> sans arguments
    size_t i = 0;
    size_t j = 0;
    int I = 0;
    int T = 0;
    int K = 0;
    int O = 0;
    int L = 0;
    int I_int = 0;
    int T_int = 0;
    int K_int = 0;
    int O_int = 0;
    int L_int = 0;
    int limit_res = 0;
    std::string password;
    std::string std_limit_res;
    std::string res;
    std::string res2;
    while(i < input.size())
    {
        if (input[i][j] == '+')
        {
            while(j < input[i].size())
            {
                if (input[i][j] == 'i')
                {
                    if (I_int == 0)
                        I_int = 1;
                    if (I < 0)
                        I--;
                    else
                        I++;
                    I *= -1;
                }
                if (input[i][j] == 't')
                {
                    if (T_int == 0)
                        T_int = 1;
                    if (T < 0)
                        T--;
                    else
                        T++;
                    T *= -1;
                }
                if (input[i][j] == 'k')
                {
                    if (K_int == 0)
                        K_int = 1;
                    if (i + 1 >= input.size())
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    if (input[i + 1][j] == '+' || input[i + 1][j] == '-')
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    // if (!(*it)->get_pass().empty())
                    // {
                    //     std::cerr << "Error : this channel has already a password" << std::endl;
                    //     return (1);
                    // }
                    if (is_valid_password(input[i + 1]) == 1)
                    {
                        //std::cerr << "Error : incorrect password definition" << std::endl;
                        return (472);
                    }
                    password = input[i + 1];
                    (*it)->set_pass(input[i + 1]);
                    if (K < 0)
                        K--;
                    else
                        K++;
                    K *= -1;
                }
                if (input[i][j] == 'o')
                {
                    if (O_int == 0)
                        O_int = 1;
                    if (i + 1 >= input.size())
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    if (input[i + 1][j] == '+' || input[i + 1][j] == '-')
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    res = is_valid_client(input, i + 1, clients, *it);
                    if (res == "442")
                        return (442);
                    if (res == "NULL")
                        return (461);
                    if (O < 0)
                        O--;
                    else
                        O++;
                    O *= -1;
                }
                if (input[i][j] == 'l')
                {
                    if (L_int == 0)
                        L_int = 1;
                    if (i + 1 >= input.size())
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    if (input[i + 1][j] == '+' || input[i + 1][j] == '-')
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    limit_res = is_number(input[i + 1]);
                    if (limit_res == 0)
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    std_limit_res = input[i + 1];
                    if (L < 0)
                        L--;
                    else
                        L++;
                    L *= -1;
                }
				else
					return 501;
                j++;
            }
            j = 0;
        }
        if (input[i][j] == '-')
        {
            while(j < input[i].size())
            {
                if (input[i][j] == 'i')
                {
                    if (I_int == 0)
                        I_int = 2;
                    if (I < 0)
                        I--;
                    else
                        I++;
                    I *= -1;
                }
                if (input[i][j] == 't')
                {
                    if (T_int == 0)
                        T = 2;
                    if (T < 0)
                        T--;
                    else
                        T++;
                    T *= -1;
                }
                if (input[i][j] == 'k')
                {
                    if (K_int == 0)
                        K_int = 2;
                    if (K < 0)
                        K--;
                    else
                        K++;
                    K *= -1;
                }
                if (input[i][j] == 'o')
                {
                    if (O_int == 0)
                        O_int = 2;
                    if (i + 1 >= input.size())
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    if (input[i + 1][j] == '+' || input[i + 1][j] == '-')
                    {
                        //std::cerr << "Error : bad arguments" << std::endl;
                        return (461);
                    }
                    res2 = is_valid_client_2(input, i + 1, clients, *it);
                    if (res == "442")
                        return (442);
                    if (res == "NULL")
                        return (461);
                    if (O < 0)
                        O--;
                    else
                        O++;
                    O *= -1;
                }
                if (input[i][j] == 'l')
                {
                    if (L_int == 0)
                        L_int = 2;
                    if (L < 0)
                        L--;
                    else
                        L++;
                    L *= -1;
                }
				else
					return 501;
                j++;
            }
            j = 0;
        }
        i++;
    }
    if (I < 0 && I_int == 1)
    {
        (*it)->set_on_invit(true);
        std::string message;
        message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + " +i";
        (*it)->send_message(message);
    }
    if (I > 0 && I_int == 2)
    {
        (*it)->set_on_invit(false);
        std::string message;
        message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + " -i";
        (*it)->send_message(message);
    }
    if (T < 0 && T_int == 1)
    {
        (*it)->set_restriction_topic(true);
        std::string message;
        message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + " +t";
        (*it)->send_message(message);
    }
    if (T > 0 && T_int == 2)
    {
        (*it)->set_restriction_topic(false);
        std::string message;
        message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + " -t";
        (*it)->send_message(message);
    }
    if (K < 0 && K_int == 1)
    {
        std::string message;
        message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + " +k " + password;
        (*it)->send_message(message);
    }
    if (K > 0 && K_int == 2)
    {
        (*it)->set_pass("");
        std::string message;
        message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + " -k";
        (*it)->send_message(message);
    }
    if (L < 0 && L_int == 1)
    {
        (*it)->set_limit(limit_res);
        std::string message;
        message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + " +l " + std_limit_res;
        (*it)->send_message(message);
    }
    if (L > 0 && L_int == 2)
    {
        (*it)->set_limit(-1);
        std::string message;
        message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + " -l ";
        (*it)->send_message(message);
    }
    if (O < 0 && O_int == 1)
    {
        std::string message;
        message = ":" + client->get_nickname() + " MODE #" + channel_name + " +o " + res;
        (*it)->send_message(message);
    }
    if (O > 0 && O_int == 2)
    {
        std::string message;
        message = ":" + client->get_nickname() + " MODE #" + channel_name + " -o " + res2;
        (*it)->send_message(message);
    }
    return (0);
}


//----------------------------- FONCTIONS UTILES ------------------------------------



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

int Command::is_number(std::string nbr)
{
    std::istringstream stream(nbr);
    int number;
    if (nbr.empty())
        return 0;
    for (size_t i = 0; i < nbr.length(); i++)
    {
        if (!isdigit(nbr[i]))
            return 0;
    }
    if (stream >> number && stream.eof())
    {
        if (number <= 0)
            return (0);
        return number;
    }
    return 0;
}

int Command::is_valid_password(std::string pass)
{
    if (pass.empty())
        return (1);
    if (pass.find(' ') != std::string::npos)
        return (1);
    if (pass.length() > 32)
        return (1);
    return (0);
}

std::string Command::is_valid_client(std::vector<std::string> input, int index, std::vector<Client*> clients, Channel *channel)
{
    size_t i = index;
    std::string client_to_verif;
    std::string res;
    res = "";
    while(i < input.size())
    {
        if (input[i][0] == '+' || input[1][0] == '-')
            return "NULL";
        client_to_verif = input[i];
        if (channel->get_client(client_to_verif) == NULL)
        {
            //std::cerr << "Error : client not in this channel" << std::endl;
            return ("442");
        }
        std::vector<Client*>::iterator it = clients.begin();
        while(it != clients.end())
        {
            if ((*it)->get_nickname() == client_to_verif || (*it)->get_username() == client_to_verif)
            {
                if (channel->get_operator(*it) != NULL)
                {
                    //std::cerr << "Error : client already operator" << std::endl;
                    break ;
                }
                channel->add_operator(*it);
                (*it)->add_channel_operator(channel);
                res = res + input[i];
                if (i + 1 != i < input.size())
                    res = res + " ";
                break ;
            }
            it++;
        }
        i++;
    }
    return res;
}

std::string Command::is_valid_client_2(std::vector<std::string> input, int index, std::vector<Client*> clients, Channel *channel)
{
    size_t i = index;
    std::string client_to_verif;
    std::string res;
    res = "";
    while(i < input.size())
    {
        if (input[i][0] == '+' || input[1][0] == '-')
            return "NULL";
        client_to_verif = input[i];
        if (channel->get_client(client_to_verif) == NULL)
        {
            //std::cerr << "Error : client not in this channel" << std::endl;
            return ("462");
        }
        std::vector<Client*>::iterator it = clients.begin();
        while(it != clients.end())
        {
            if ((*it)->get_nickname() == client_to_verif || (*it)->get_username() == client_to_verif)
            {
                if (channel->get_operator(*it) == NULL)
                {
                    //std::cerr << "Error : client not operator" << std::endl;
                    break ;
                }
                channel->supp_operator(*it);
                (*it)->supp_channel_operator(channel);
                res = res + input[i];
                if (i + 1 != i < input.size())
                    res = res + " ";
                break ;
            }
            it++;
        }
        i++;
    }
    return (res);
}

int Command::verif_mode_char(std::vector<std::string> input)
{
	size_t i = 0;
	size_t j = 0;
	while(i < input.size())
	{
		if (input[i][0] == '-' || input[i][0] == '+')
		{
			j = 1;
			while (j < input[i].size())
			{
				if (input[i][j] != 'k' && input[i][j] != 'o'
						&& input[i][j] != 'l' && input[i][j] != 'i' && input[i][j] != 't')
					return 1;
				j++;
			}
		}
		i++;
	}
	return 0;
}