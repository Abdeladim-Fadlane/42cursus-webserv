/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configfile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:14:04 by akatfi            #+#    #+#             */
/*   Updated: 2024/02/02 17:25:12 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <algorithm>
#include <map>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include <string>


class Location
{
    public:
        std::string                 location_name;
        std::string                 root;
        std::string                 autoindex;
        std::vector<std::string>    indexs;
        std::vector<std::string>    allowed_method;
        std::string                 uploadfile;
        std::string                 upload_location;
        std::string                 cgi_allowed;
        std::string                 redirection;
        std::map<std::string, std::string>    cgi;
        bool                        close;
        Location(std::string& location_name);
        Location& operator=(const Location& obj);
        Location();
        void setLocationName(const std::string& new_name);
        void add_location(std::fstream& os);
        ~Location();
};

class Server
{
    public:
        long                                listen;
        std::string                         host;
        size_t                              cgi_timeout;
        std::string                         server_name;
        std::map<long, std::string>  error_pages;
        long                                max_body;
        std::vector<Location>               locations;
        bool                                close;
        bool                                port_chose;
        std::vector<long>                   error_auth;
        Server();
        Server& operator=(const Server& obj);
        ~Server();
        void    init_data(std::fstream& os);
        long    init_numberError(std::string path);
};

class ConfigFile
{
    private:
        std::fstream        config;
    public:
        std::vector<Server> Servers;
        ConfigFile(const std::string& FileName);
        void close_and_throw(std::string msg_error);
        void    parceConfig();
        ~ConfigFile();
};
bool getlineFromFile(std::fstream& os, std::string& input);
std::vector<std::string> split_line(std::string line);
bool check_digit(std::string digit);
std::string delete_Or_add_slash(std::string& path, bool begin, bool end);

#endif