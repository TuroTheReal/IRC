/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artberna <artberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 12:37:46 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/16 14:50:06 by artberna         ###   ########.fr       */
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
	if (ac != 3)
		return std::cerr << "Usage: ./ircserv <port> <password>" << std::endl, 1;

	if (!isValid(av[1]))
		return std::cerr << "Error: Wrong port: " << av[1] << std::endl, 1;

	int port = atoi(av[1]);
	if (port <= 0 || port > 65535)
		return std::cerr << "Error: Cannot use this port" << std::endl, 1;

	std::string password = av[2];
	if (password.empty())
		return std::cerr << "Error : no password" << std::endl, 1;
	Server IRC(port, password);
	return (0);
}
