/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 13:07:04 by dsindres          #+#    #+#             */
/*   Updated: 2025/05/20 11:46:44 by dsindres         ###   ########.fr       */
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
                    message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost KICK #" + channel_name + " " + test->get_nickname() + " :" + input[3];
                    (*it)->send_message(message);
                }
                else
                {
                    //std::cout << input[2] << " is ejected from " << channel_name << " channel" << std::endl;
                    std::string message;
                    message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost KICK #" + channel_name + " " + test->get_nickname();
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
                message2 = ":" + client->get_nickname() + "!~" + client->get_username() + "@localhost INVITE " + (*ite)->get_nickname() + " :#" + channel_name;
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

int Command::mode(std::vector<std::string> input, std::vector<Client*> clients, std::vector<Channel*> channels, Client *client)
{
    (void)clients;

    if (input.size() < 2)
    {
        //std::cout << "LAAAAAAA 1" << std::endl;
        return 461; // ERR_NEEDMOREPARAMS
    }

    std::string channel_name = input[1];
    channel_name.erase(0, 1);


    std::vector<Channel*>::iterator it = channels.begin();
    while (it != channels.end()) {
        if (channel_name == (*it)->get_name()) {
            break;
        }
        it++;
    }

    if (it == channels.end())
    {
        //std::cout << "LAAAAAAA 2" << std::endl;
        return 403; // ERR_NOSUCHCHANNEL
    }

    if (input.size() >= 2) {
        input.erase(input.begin(), input.begin() + 2);
    }


    if (input.empty())
    {
        std::string message01 = ":IRC 324 " + client->get_nickname() + " #" + channel_name + " +";
        std::string message02;
        std::string mdp = (*it)->get_pass();
        if (mdp != "")
            message02 += "k";
        int res_limit = (*it)->get_limit();
        if (res_limit != -1)
            message02 += "l";
        if ((*it)->get_on_invit() == true)
            message02 += "i";
        if ((*it)->get_restriction_topic() == true)
            message02 += "t";
        if (mdp != "")
            message02 = message02 + " " + mdp;
        if (res_limit != -1)
        {
            std::stringstream ss;
            ss << res_limit;
            std::string result = ss.str();
            message02 = message02 + " " + result;
        }
        std::string message = message01 + message02;
        client->receive_message(message, client->get_socket());
        std::stringstream sstime;
        sstime << (*it)->getCreationTime();
        std::string time = sstime.str();
        std::string messtime = ":IRC 329 " + client->get_nickname() + " #" + channel_name + " " + time;
        client->receive_message(messtime, client->get_socket());
        return 0;
    }

    if (input[0][0] != '+' && input[0][0] != '-')
    {
        //std::cout << "LAAAAAAA 4" << std::endl;
        return 461; // ERR_NEEDMOREPARAMS
    }

    if (verif_mode_o(input) == 1)
    {
        //std::cout << "verif mode o exit" << std::endl;
        return 501; // ERR_UMODEUNKNOWNFLAG
    }

    if (verif_mode_char(input) == 1)
    {
        //std::cout << "verif mode char exit" << std::endl;
        return 501; // ERR_UMODEUNKNOWNFLAG
    }


    struct ModeState {
        bool set;           // true si mode à définir, false si à retirer
        bool processed;     // indique si le mode a été traité
        int index;          // index utilisé pour les modes avec paramètres
        std::string param;  // paramètre associé au mode
    };

    // Initialiser l'état des modes
    ModeState iMode = {false, false, 0, ""};  // Mode invitation
    ModeState tMode = {false, false, 0, ""};  // Mode restriction topic
    ModeState kMode = {false, false, 0, ""};  // Mode mot de passe
    ModeState oMode = {false, false, 0, ""};  // Mode opérateur
    ModeState lMode = {false, false, 0, ""};  // Mode limite

    size_t i = 0;
    int o_stop = 0;
    while (i < input.size())
    {
        bool isSet = (input[i][0] == '+');
        int flag = 0;


        for (size_t j = 1; j < input[i].size(); j++)
        {
            //int o_stop = 0;
            char mode = input[i][j];

            switch (mode) {
                case 'i':
                    iMode.set = isSet;
                    iMode.processed = true;
                    break;

                case 't':
                    tMode.set = isSet;
                    tMode.processed = true;
                    break;

                case 'k':
                    kMode.set = isSet;
                    kMode.processed = true;
                    kMode.index = i;

                    if (isSet)
                    {
                        size_t paramIndex = 0;
                        size_t w = i + 1 + flag;
                        int found = after_w(input[i], j, 0);
                        while(w < input.size() && input[w][0] != '-' && input[w][0] != '+')
                            w++;
                        if (w - 1 - found < i + 1 + flag)
                        {
                            return 461;
                        }
						else if (w - 1 - found > i + 1 + flag)
                        {
                            return 459;
                        }
                        if (found == 0)
                            paramIndex = i + 1 + flag;
                        else
                            paramIndex = i + 1;
                        if (is_valid_password(input[paramIndex]) == 0)
                            return 472;

                        // Stocker le paramètre
                        kMode.param = input[paramIndex];

                        flag++;
                    }
                    break;

                case 'o':
                    oMode.set = isSet;
                    oMode.processed = true;
                    oMode.index = i;

                    if (i + 1 >= input.size()) {
                        return 461;
                    }

                    if (isSet)
                    {
                        std::string validClientsList;
                        size_t clientIndex = i + 1;
                        Client* targetClient = (*it)->get_client(input[clientIndex]);
                        
                        if (targetClient == NULL)
                            return (441);
                            
                        if ((*it)->get_operator(targetClient) != NULL)
                            return (467);

                        // Ajouter l'opérateur
                        (*it)->add_operator(targetClient);
                        targetClient->add_channel_operator(*it);
                        targetClient->set_operator(true);

                        validClientsList = input[clientIndex];
                        oMode.param = validClientsList;
                    }
                    else
                    {
                        std::string validClientsList;
                        size_t clientIndex = i + 1;
                        Client* targetClient = (*it)->get_client(input[clientIndex]);
                        
                        if (targetClient == NULL)
                            return (441);

                        if ((*it)->get_operator(targetClient) == NULL)
                            return (468);

                        (*it)->supp_operator(targetClient);
                        targetClient->supp_channel_operator(*it);
                        if (targetClient->operator_vector_nul() == 1)
                            targetClient->set_operator(false);

                        validClientsList = input[clientIndex];
                        oMode.param = validClientsList;
                    }
                    o_stop = 1;
                    break;

                case 'l':
                    lMode.set = isSet;
                    lMode.processed = true;
                    lMode.index = i;

                    if (isSet)
                    {
                        size_t w = i + 1 + flag;
                        int found = after_w(input[i], j, 1);

                        while(w < input.size() && input[w][0] != '-' && input[w][0] != '+')
                            w++;
						if (w - 1 - found < i + 1 + flag)
                            return 461;
						else if (w - 1 - found > i + 1 + flag)
                            return 459;

                        int limit;
                        if (found == 0)
                            limit = is_number(input[i + 1 + flag]);
                        else
                            limit = is_number(input[i + 1]);
                        if (limit == 0)
                            return 477;
                        if (found == 0)
                            lMode.param = input[i + 1 + flag];
                        else
                            lMode.param = input[i + 1];
                        flag++;
                    }
                    break;

            }
        }
        if (o_stop != 0)
            break;
        if (flag != 0)
            i += flag;
        i++;
    }

    if (iMode.processed) {
        (*it)->set_on_invit(iMode.set);
        std::string message = ":" + client->get_nickname() + "!" + client->get_username() +
                             "@localhost MODE #" + channel_name + (iMode.set ? " +i" : " -i");
        (*it)->send_message(message);
    }

    if (tMode.processed) {
        (*it)->set_restriction_topic(tMode.set);
        std::string message = ":" + client->get_nickname() + "!" + client->get_username() +
                             "@localhost MODE #" + channel_name + (tMode.set ? " +t" : " -t");
        (*it)->send_message(message);
    }

    if (kMode.processed) {
        if (kMode.set) {
            (*it)->set_pass(kMode.param);
            std::string message = ":" + client->get_nickname() + "!" + client->get_username() +
                                "@localhost MODE #" + channel_name + " +k " + kMode.param;
            (*it)->send_message(message);
        } else {
            (*it)->set_pass("");
            std::string message = ":" + client->get_nickname() + "!" + client->get_username() +
                                "@localhost MODE #" + channel_name + " -k";
            (*it)->send_message(message);
        }
    }

    if (lMode.processed) {
        if (lMode.set) {
            int limit = is_number(lMode.param);
            (*it)->set_limit(limit);
            std::string message = ":" + client->get_nickname() + "!" + client->get_username() +
                                "@localhost MODE #" + channel_name + " +l " + lMode.param;
            (*it)->send_message(message);
        } else {
            (*it)->set_limit(-1);
            std::string message = ":" + client->get_nickname() + "!" + client->get_username() +
                                "@localhost MODE #" + channel_name + " -l";
            (*it)->send_message(message);
        }
    }

    if (oMode.processed)
    {
        if (oMode.param != "")
        {
            std::string message = ":" + client->get_nickname() + "!" + client->get_username() + "@localhost MODE #" + channel_name + (oMode.set ? " +o " : " -o ") + oMode.param;
            (*it)->send_message(message);
        }
    }

    return 0;
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
	std::string prohibed;

	if (pass.size() > 20 || pass.empty())
		return 0;

	for (char c = 0; c < 32; ++c)
		prohibed += c;
	prohibed += " ,*?!@.\'\"{[($#@!%^&*|<>)]}";

	for (size_t i = 0; i < pass.size(); i++){
		if (prohibed.find(pass[i]) != std::string::npos){
			return 0;
		}
	}
	return 1;
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
            if ((*it)->get_nickname() == client_to_verif)
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
    //if (res != "")
        return res;
    //else
       // return
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
            if ((*it)->get_nickname() == client_to_verif)
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

int Command::verif_mode_o(std::vector<std::string> input)
{
    int flag = 0;
    int flag2 = 0;
    for (size_t i = 0; i < input.size(); i++)
    {
        if (input[i][0] == '+' || input[i][0] == '-')
        {
            for (size_t j = 1; j < input[i].size(); j++)
            {
                if (input[i][j] == 'o')
                    flag++;
                if (input[i][j] != 'o')
                    flag2++;
            }
        }
    }
    if (flag == 0)
        return 0;
    if (flag != 1)
        return 1;
    if (flag != 0 && flag2 != 0)
        return 1;
    return 0;
}

int Command::verif_mode_char(std::vector<std::string> input)
{
    for (size_t i = 0; i < input.size(); i++)
    {
        // Vérifier si la chaîne est vide
        if (input[i].empty())
            return 1;

        // Vérifier si la chaîne commence par '+' ou '-'
        if (input[i][0] == '+' || input[i][0] == '-')
        {
            // Si la chaîne n'a que le caractère '+' ou '-' sans autres caractères, c'est invalide
            if (input[i].size() == 1)
                return 1;

            // Tableau pour suivre les modes déjà vus
            bool seen_modes[256] = {false}; // Tableau assez grand pour tous les caractères ASCII
            bool has_o_mode = false;
            bool has_other_modes = false;

            // Vérifier chaque caractère après le premier
            for (size_t j = 1; j < input[i].size(); j++)
            {
                char current_mode = input[i][j];

                // Vérifier si le caractère est un mode valide
                if (current_mode != 'k' && current_mode != 'o' &&
                    current_mode != 'l' && current_mode != 'i' && current_mode != 't')
                {
                    return 1;
                }

                // Vérifier si ce mode a déjà été vu dans cette chaîne
                if (seen_modes[(unsigned char)current_mode])
                {
                    // Le mode apparaît plus d'une fois, c'est invalide
                    return 1;
                }

                // Vérifier si c'est le mode "o"
                if (current_mode == 'o')
                {
                    has_o_mode = true;
                }
                else
                {
                    has_other_modes = true;
                }

                // Marquer le mode comme vu
                seen_modes[(unsigned char)current_mode] = true;
            }

            // Vérifier que le mode "o" est seul s'il est présent
            if (has_o_mode && has_other_modes)
            {
                return 1; // Le mode "o" n'est pas seul
            }

            // Si le mode "o" est le seul mode, vérifier qu'il est effectivement seul (pas d'autres caractères)
            if (has_o_mode && input[i].size() > 2)
            {
                return 1; // Il y a plus d'un caractère de mode avec le mode "o"
            }
        }
        // else
        // {
        //     // Si la chaîne ne commence pas par '+' ou '-', c'est invalide
        //     return 1;
        // }
    }
    return 0;
}

int Command::after_w(std::string input, size_t index, int flag)
{
    int i = 0;
    while (index < input.size())
    {
        if (flag == 0)
        {
            if (input[index] == 'l')
                i++;
        }
        if (flag == 1)
        {
            if (input[index] == 'k')
                i++;
        }
        if (flag == 2)
        {
            if (input[index] == 'k' || input[index] == 'l')
                i++;
        }
        index++;
    }
    return (i);
}
