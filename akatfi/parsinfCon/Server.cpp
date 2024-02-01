/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 18:34:11 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/31 19:10:17 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configfile.hpp"

long Server::init_numberError(std::string path)
{
    int i = 0;
    while (i< path.length() && isdigit(path[i]))
        i++;
    return (atoi(path.substr(0, i).c_str()));
}

Server::Server()
{
    port_chose = false;
    close = true;
    DIR* dir_error = opendir("afadlane");
    dirent* path;
    if(! dir_error)
        throw std::runtime_error("error open dir");
    while ((path = readdir(dir_error)))
    {
        if (path->d_name[0] != '.')
            error_pages[init_numberError(std::string(path->d_name))] = std::string("Error_pages").append(path->d_name);
        // std::cout << path->d_name << std::endl;
    }
}

void    Server::init_data(std::fstream& os)
{
    std::string input;
    std::vector<std::string> arg;
    std::vector<Location>::iterator it;

    while (getlineFromFile(os, input) && input != "server")
    {
        arg = split_line(input);
        if (arg[0][0] != '}' && arg[0][0] != '{' && arg[0] != "location")
        {   
            if (arg[arg.size() - 1][arg[arg.size() - 1].size() - 1] != ';')
                throw std::runtime_error("Error : the line will has a ;");
            arg[arg.size() - 1] = arg[arg.size() - 1].substr(0, arg[arg.size() - 1].size() - 1);
        }
        if (!arg[0].compare("{") && arg.size() == 1)
            close = false;
        else if (!arg[0].compare("}") && arg.size() == 1)
        {
            close = true; 
            break ;
        }
        else if (!arg[0].compare("listen") && arg.size() == 2 && !close)
        {
            if (!check_digit(arg[1]))
                throw std::runtime_error("Error : thee port will be a digit");
            listen = atoi(arg[1].c_str());
            port_chose = true;
            if (!(listen >= 0 && listen <= 65535))
                throw std::runtime_error("Error : the port is not available");
        }
        else if (!arg[0].compare("host") && arg.size() == 2 && !close)
            host = arg[1];
        else if (!arg[0].compare("server_name") && arg.size() == 2 && !close)
            server_name = arg[1];
        else if (!arg[0].compare("max_body_Size") && arg.size() == 2 && !close)
        {
            if (!check_digit(arg[1]) || !(atoi(arg[1].c_str()) > 0 && atoi(arg[1].c_str()) <= 2147483647))
                throw std::runtime_error("Error : the max body size will be a digit and between 1 and 2147483647");
            max_body = atoi(arg[1].c_str());
        }
        else if (!arg[0].compare("error_page") && !close && arg.size() == 3)
        {
            if (!check_digit(arg[1]) || !(atoi(arg[1].c_str()) >= MIN_PAGE && atoi(arg[1].c_str()) <= MAX_PAGE))
                throw std::runtime_error("Error : thee number of error_page will be a digit between 400 and 406");
            error_pages[atoi(arg[1].c_str())] = arg[2];
        }
        else if (!arg[0].compare("location") && arg.size() == 2 && !close)
        {
            locations.push_back(Location(arg[1]));
            it = locations.end() - 1;
            it->add_location(os);
        }
        else
            throw std::runtime_error("Error : line have dosen't folow rules");
    }
    if (!close)
        throw std::runtime_error("Error : The brackets of server dosen't closed");
}

Server::~Server()
{}