/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 12:37:46 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/22 16:28:11 by artberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

bool isValid(const char *str){
	std::string st = str;
	for (size_t i = 0; i < st.size(); i++)
	{
		if (!(st[i] >= '0' && st[i] <= '9'))
			return 0;
	}
	return 1;
}

int main(int ac, char **av)
{
	try {
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");

		if (!isValid(av[1]))
			throw std::runtime_error("Wrong port: " + std::string(av[1]));

		int port = atoi(av[1]);
		if (port <= 0 || port > 65535)
			throw std::runtime_error("Cannot use this port");

		std::string password = av[2];
		if (password.empty())
			throw std::runtime_error("No password");

		Server IRC(port, password);
	}
	catch (std::exception &e){
		std::cerr << "Error: " << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (0);
}
