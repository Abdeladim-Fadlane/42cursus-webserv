/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:13:54 by akatfi            #+#    #+#             */
/*   Updated: 2024/02/02 18:29:42 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configfile.hpp"

ConfigFile::ConfigFile(const std::string& FileName)
{
    struct stat fileStat;
    
    config.open(FileName.c_str());
    if (!config.is_open())
        throw std::runtime_error("Error : cant opening this file");
    stat(FileName.c_str(), &fileStat);
    if (fileStat.st_size == 0)
        close_and_throw("Error : the file is empty");
}

bool check_digit(std::string digit)
{
    for (unsigned int i = 0; i < digit.length(); i++)
        if (!isdigit(digit[i]))
            return false;
    return true;
}

bool getlineFromFile(std::fstream& os, std::string& input)
{
    if (os.eof() == true)
        return false;
    std::getline(os, input);
    while (!input.empty() && isspace(input[0]))
        input = input.substr(1);
    while (!input.empty() && isspace(input[input.length() - 1]))
        input = input.substr(0, input.length() - 1);
    return true;
}

void ConfigFile::close_and_throw(std::string msg_error)
{
    config.close();
    throw std::runtime_error(msg_error);
}

std::vector<std::string> split_line(std::string line)
{
    std::vector<std::string> words;
    size_t i = 0;
    size_t end;
    int    start;
     
    while (i < line.length())
    {
        if (!isspace(line[i]))
        {
            end = 0;
            start = i;
            while (i < line.length() && !isspace(line[i++]))
                end++;
            words.push_back(line.substr(start, end));
        }
        else if (isspace(line[i]))
            i++;
        else
            throw std::runtime_error("Error");
    }
    return (words);
}

std::string delete_Or_add_slash(std::string& path, bool begin, bool end)
{
    while (path.size() > 0 && path[0] == '/')
        path = path.substr(1);
    while (path.size() > 0  && path[path.size() - 1] == '/')
        path = path.substr(0, path.size() - 1);
    if (begin == true)
        path = "/" + path;
    if (end == true)
        path += "/";
    return (path);
}

void    ConfigFile::parceConfig()
{
    std::string input;
    std::vector<std::string> line_arg;
    std::vector<Server>::iterator it;
    
    while(getlineFromFile(config, input))
    {
        if (input.empty() == true)
            continue ;
        line_arg = split_line(input);
        if (!line_arg[0].compare("server") && line_arg.size() == 1)
        {
            try
            {
                Servers.push_back(Server());
                it = Servers.end() - 1;
                it->init_data(config);
                if (it->host.empty() || !it->port_chose)
                    throw std::runtime_error("Error : the server need host and port");
                for (size_t i = 0; i < Servers.size() - 1; i++)
                {
                    if (Servers[i].host == it->host && Servers[i].listen == it->listen && it->server_names.size() == 0)
                        throw std::runtime_error("Error : two server pr more have the same name and the same port and dosen't have the server names");
                    else if (Servers[i].host == it->host && Servers[i].listen == it->listen)
                    {
                        for (size_t j = 0; j < it->server_names.size(); j++)
                            if (std::find(Servers[i].server_names.begin(), Servers[i].server_names.end(), it->server_names[j]) != Servers[i].server_names.end())
                                throw std::runtime_error("Error : one or more server has the same port, host and server name");
                    }
                }
            }
            catch(const std::exception& e)
            {
                close_and_throw(e.what());
            }
            
        }
        else
            close_and_throw("Error : line have dosen't follow rule");
    }
    config.close();
}

ConfigFile::~ConfigFile()
{
    if (config.is_open())
        config.close();
}