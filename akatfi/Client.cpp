/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/29 11:51:02 by akatfi            #+#    #+#             */
/*   Updated: 2023/12/29 11:58:46 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd) : Requeste(fd)
{
    read_access = true;
    write_access = false;
}

bool    Client::getReadAccess()
{
    return (this->read_access);
}

bool    Client::getWriteAccess()
{
    return (this->write_access);
}