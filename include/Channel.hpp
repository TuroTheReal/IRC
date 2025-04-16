/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:25:46 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/16 14:49:31 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <string>
#include <memory>
#include "Client.hpp"

class Channel
{
	public:
		Channel(std::string name);
		Channel(Channel const &other);
		Channel &operator=(Channel const &other);
		~Channel();
		
		void add_client(std::shared_ptr<Client> client);
		void remove_client(std::shared_ptr<Client> client);
		void send_message(const std::string &message);

	private:
		std::string _name;
		std::vector<Client> _clients;
};