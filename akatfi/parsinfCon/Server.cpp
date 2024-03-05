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
    long number_error;
    max_body = LONG_MAX;
    cgi_timeout = 10;
    DIR* dir_error = opendir("akatfi/parsinfCon/error");
    if (!dir_error)
        throw std::runtime_error("Error : the path of page error not found");
    dirent* path;
    while ((path = readdir(dir_error)))
        if (path->d_name[0] != '.')
        {
            number_error = init_numberError(std::string(path->d_name));
            error_pages[number_error] = std::string("akatfi/parsinfCon/error/").append(path->d_name);
            if (number_error != 201)
                error_auth.push_back(number_error);
        }
    closedir(dir_error);
}

Server& Server::operator=(const Server& obj)
{
    listen = obj.listen;
    host = obj.host;
    server_names = obj.server_names;
    error_pages = obj.error_pages;
    max_body = obj.max_body;
    locations = obj.locations;
    close = obj.close;
    port_chose = obj.port_chose;
    cgi_timeout = obj.cgi_timeout;
    return (*this);
}

void    Server::init_data(std::fstream& os)
{
    std::string input;
    std::vector<std::string> arg;
    std::vector<Location>::iterator it;
    std::vector<std::string> number;
    std::stringstream   ss;
    std::string         p;

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
            if (!check_digit(arg[1]) && !(atoi(arg[1].c_str()) >= 1 && atoi(arg[1].c_str()) <= 30))
                throw std::runtime_error("Error : the timeout will be a digit and between 1 and 30");
            cgi_timeout = atoi(arg[1].c_str());
        }
        else if (!arg[0].compare("host") && arg.size() == 2 && !close && host.empty())
        {
            ss << arg[1];
            while (std::getline(ss, p, '.'))
                if (check_digit(p))
                {
                    if (!(atoi(p.c_str()) >= 0 && atoi(p.c_str()) <= 256))
                        break ;
                    number.push_back(p);
                }
            if (number.size() != 4)
                throw std::runtime_error("Error : the host is nont valid");
            host = arg[1];
        }
        else if (!arg[0].compare("server_names") && arg.size() >= 2  && !close && server_names.size() == 0)
        {
            for (size_t i = 1; i < arg.size(); i++)
                server_names.push_back(arg[i]);
        }
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
            if (std::find(error_auth.begin(), error_auth.end() ,atoi(arg[1].c_str())) == error_auth.end())
                throw std::runtime_error("Error : the number " + arg[1] + " of error_page is not authorized in this configuration file");
            if (access(arg[2].c_str(), R_OK) == 0)
                error_pages[atoi(arg[1].c_str())] = arg[2];
        }
        else if (!arg[0].compare("location") && arg.size() == 2 && !close)
        {
            locations.push_back(Location(arg[1]));
            it = locations.end() - 1;
            it->add_location(os);
            for (size_t i = 0; i < locations.size() - 1; i++)
                if (locations[i].location_name == it->location_name)
                    throw std::runtime_error("Error : there is two or more location have the same name");
        }
        else
            throw std::runtime_error("Error : the Server have dosen't follow rules or duplicated");
    }
    if (!close)
        throw std::runtime_error("Error : The brackets of server dosen't closed");
    if (locations.size() == 0)
        throw std::runtime_error("Error : the Server dosen't has any location");
}

Server::~Server()
{}