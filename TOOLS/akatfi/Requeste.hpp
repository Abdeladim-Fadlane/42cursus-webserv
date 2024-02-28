/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requeste.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:13:07 by akatfi            #+#    #+#             */
/*   Updated: 2024/02/04 16:03:24 by akatfi           ###   ########.fr       */
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
#include <fcntl.h>
#include <sys/time.h>
#include <algorithm>
#include "parsinfCon/Configfile.hpp"
class PostMethod;
class Requeste
{
    private:
        int                                 fd_socket;
        std::string                         head;
        std::string                         body;
        ConfigFile                          &config;
        int                                 fdresponse;
        bool                                chose_location;
    public:
        long                                time_out;
        bool                                skeeptime_out;
        std::string                         headerResponse;
        Server                              Server_Requeste;
        Location                            Location_Server;
        long                                status_client;
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
        std::string                         file_name;
        bool                                done;
        Requeste(int fd, ConfigFile &config);
        void    MakeMapOfHeader(bool& isdone);
        void set_status_client(bool&);
        void readFromSocketFd(bool &isdone, bool &flag);
        std::pair<std::string, std::string> MakePair(std::string& line);
        void    get_infoConfig(bool& isdone);
        int     getSocketFd() const;
        const std::string& getBody() const;
        const std::string&    getPath() const;
        long    get_time();
        ~Requeste();
};

void setFileextation(std::string const& f, std::map<std::string, std::string>& Extation);

#endif