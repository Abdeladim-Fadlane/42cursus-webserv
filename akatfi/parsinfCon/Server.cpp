/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 18:34:11 by akatfi            #+#    #+#             */
/*   Updated: 2024/02/02 18:29:30 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configfile.hpp"

long Server::init_numberError(std::string path)
{
    unsigned int i = 0;
    while (i < path.length() && isdigit(path[i]))
        i++;
    return (atoi(path.substr(0, i).c_str()));
}

Server::Server()
{
    port_chose = false;
    close = true;
    max_body = LONG_MAX;
    cgi_timeout = 10;
    DIR* dir_error = opendir("akatfi/parsinfCon/error");
    dirent* path;
    while ((path = readdir(dir_error)))
        if (path->d_name[0] != '.')
            error_pages[init_numberError(std::string(path->d_name))] = std::string("akatfi/parsinfCon/error/").append(path->d_name);
    closedir(dir_error);
}

Server& Server::operator=(const Server& obj)
{
    listen = obj.listen;
    host = obj.host;
    server_name = obj.server_name;
    error_pages = obj.error_pages;
    max_body = obj.max_body;
    locations = obj.locations;
    close = obj.close;
    port_chose = obj.port_chose;
    return (*this);
}

void    Server::init_data(std::fstream& os)
{
    std::string input;
    std::vector<std::string> arg;
    std::vector<Location>::iterator it;

    while (getlineFromFile(os, input) && input != "server")
    {
        if (input.empty() == true)
            continue;
        if (input != "}" && input != "{" && input.find("location") == std::string::npos)
        {
            if (input[input.length() - 1] != ';')
                throw std::runtime_error("Error : the line will has a ;");
            input = input.substr(0, input.length() - 1);
        }
        arg = split_line(input);
        if (!arg[0].compare("{") && arg.size() == 1)
            close = false;
        else if (!arg[0].compare("}") && arg.size() == 1)
        {
            close = true; 
            break ;
        }
        else if (!arg[0].compare("listen") && arg.size() == 2 && !close && port_chose == false)
        {
            if (!check_digit(arg[1]))
                throw std::runtime_error("Error : thee port will be a digit");
            listen = atoi(arg[1].c_str());
            port_chose = true;
            if (!(listen >= 0 && listen <= 65535))
                throw std::runtime_error("Error : the port is not available");
        }
        else if (!arg[0].compare("cgi_timeout") && arg.size() == 2 && !close)
        {
            if (!check_digit(arg[1]))
                throw std::runtime_error("Error : thee timeout will be a digit");
            cgi_timeout = atoi(arg[1].c_str());
        }
        else if (!arg[0].compare("host") && arg.size() == 2 && !close && host.empty())
            host = arg[1];
        else if (!arg[0].compare("server_name") && arg.size() == 2 && !close && server_name.empty())
            server_name = arg[1];
        else if (!arg[0].compare("max_body_Size") && arg.size() == 2 && !close)
        {
            if (!check_digit(arg[1]) || !(atoi(arg[1].c_str()) > 0 && atoi(arg[1].c_str()) <= 2147483647))
                throw std::runtime_error("Error : the max body size will be a digit and between 1 and 2147483647");
            max_body = atoi(arg[1].c_str());
        }
        else if (!arg[0].compare("error_page") && !close && arg.size() == 3)
        {
            if (!check_digit(arg[1]))
                throw std::runtime_error("Error : the number of error_page will be a digit");
            if (access(arg[2].c_str(), R_OK) == 0)
                error_pages[atoi(arg[1].c_str())] = arg[2];
        }
        else if (!arg[0].compare("location") && arg.size() == 2 && !close)
        {
            locations.push_back(Location(arg[1]));
            it = locations.end() - 1;
            it->add_location(os);
        }
        else
            throw std::runtime_error("Error : line have dosen't folow rules or duplicated");
    }
    if (!close)
        throw std::runtime_error("Error : The brackets of server dosen't closed");
}

Server::~Server()
{}