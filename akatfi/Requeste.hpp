/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requeste.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:13:07 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/31 20:38:52 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTE_HPP
#define REQUESTE_HPP
#include <iostream>
#include <map>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include "parsinfCon/Configfile.hpp"
class PostMethod;
class Requeste
{
    private:
        int                                 fd_socket;
        std::string                         head;
        std::string                         body;
        ConfigFile                          &config;
    public:
        Location                            locationServer;
        int                                 port;
        std::string                         query_str;
        std::string                         content_type;
        std::string                         content_length;
        std::string                         host;
        std::string                         path;
        PostMethod                          *post;
        std::string                         http_v;
        std::string                         method;
        std::map<std::string, std::string>  requeste_map;
        Requeste(int fd, ConfigFile &config);
        void    MakeMapOfHeader();
        void readFromSocketFd(bool &flag);
        std::pair<std::string, std::string> MakePair(std::string& line);
        void    get_infoConfig();
        std::map<std::string, std::string> getRequesteMap();
        int     getSocketFd() const;
        const std::string& getBody() const;
        const std::string&    getPath() const;
        ~Requeste();
};

void setFileextation(std::string const& f, std::map<std::string, std::string>& Extation);

#endif