/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/29 11:54:13 by akatfi            #+#    #+#             */
/*   Updated: 2023/12/29 11:57:23 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "requeste.hpp"

class Client : public requeste
{
    private:
        bool    read_access;
        bool    write_access;
    public:
        Client(int fd);
        bool    getReadAccess();
        bool    getWriteAccess();
};

#endif