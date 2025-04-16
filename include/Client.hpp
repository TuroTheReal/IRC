/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:25:10 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/15 09:58:19 by dsindres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <memory>
#include <list>
#include <netinet/in.h>
 
class Client
{
    public:
        Client();
        Client(int socket, std::string nickname);
        Client(Client const &other);
        Client &operator=(Client const &other);
        ~Client();
        
        void send_message(const std::string &message);
        void receive_message();
        
    private:
        int _socket;
        std::string _nickname;
        std::string _password;
        sockaddr_in _address;
        bool _isAuthenticated;
        std::list<std::string> _channels; 
};