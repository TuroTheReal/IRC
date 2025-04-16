/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsindres <dsindres@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:26:09 by dsindres          #+#    #+#             */
/*   Updated: 2025/04/14 13:31:18 by dsindres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <memory>

class Command
{
    public:
        Command(std::string raw_command);
        void execute(std::shared_ptr<Client> client);

    private:
        std::string raw_command;
};