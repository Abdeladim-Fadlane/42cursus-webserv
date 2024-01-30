/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:13:54 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/29 14:06:57 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configfile.hpp"

ConfigFile::ConfigFile(const std::string& FileName)
{
    std::stringstream ss;
    struct stat fileStat;
    
    config.open(FileName.c_str());
    if (!config.is_open())
        throw std::runtime_error("Error : cant opening this file");
    stat(FileName.c_str(), &fileStat);
    if (fileStat.st_size == 0)
        throw std::runtime_error("Error the file is empty");

    // dont forget to set up the paths of file errors
    // for (unsigned int i = 400; i <= 406; i++)
    // {
    //     ss << i;
    //     error[i] = std::string("Error_pages/") + ss.str() + ".html";
    //     std::stringstream().swap(ss);
    // }
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
    if (input.empty())
        throw std::runtime_error("Error : th config file has a empty line");
    // std::cout << "---> " << input << std::endl;
    return true;
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

void    ConfigFile::checkErrorPages(std::vector<Server>::iterator serv)
{
    std::map<long, std::string>::iterator it1;
    struct stat fileStat;
    
    for (std::map<long, std::string>::iterator it = error.begin(); it != error.end(); it++)
    {
        it1 = serv->error_pages.find(it->first);
        if (it == serv->error_pages.end())
            serv->error_pages[it->first] = it->second;
        else 
        {
            stat(it1->second.c_str(), &fileStat);
            if (access(it->second.c_str(), F_OK) == -1 || !fileStat.st_size)
                it1->second = it->second;
        }
    }
}

void    ConfigFile::parceConfig()
{
    std::string input;
    std::vector<std::string> line_arg;
    std::vector<Server>::iterator it;
    
    while(getlineFromFile(config, input))
    {
        line_arg = split_line(input);
        if (!line_arg[0].compare("server") && line_arg.size() == 1)
        {
            Servers.push_back(Server());
            it = Servers.end() - 1;
            it->init_data(config);
            this->checkErrorPages(it);
            if (it->host.empty() || !it->port_chose)
                throw std::runtime_error("Error : the server need host and post");
            for (std::vector<Server>::iterator itServ = Servers.begin(); itServ != Servers.end() - 1; itServ++)
                if (it->host == itServ->host && it->listen == itServ->listen)
                    throw std::runtime_error("Error : one server have port and host similar to other server");
            // int i = 0;
            // std::cout << "--------------  server  -------------------" << std::endl;
            // if (it->listen)
            //     std::cout << "listen : " << it->listen<< std::endl;
            // if (!it->host.empty())
            //     std::cout << "host : " << it->host<< std::endl;
            // if (!it->server_name.empty())
            //     std::cout << "server_name : " << it->server_name<< std::endl;
            // if (!it->max_body.empty())
            //     std::cout << "max_body : " << it->max_body<< std::endl;
            //  for (std::vector<Location>::iterator it1 = it->locations.begin(); it1 != it->locations.end(); it1++)
            // {
            //     std::cout << "----------- location" << ++i << " ----------------" << std::endl;
            //     if (!it1->location_name.empty())
            //         std::cout << "location_name : " << it1->location_name<< std::endl;
            //     if (!it1->root.empty())
            //         std::cout << "root : " << it1->root<< std::endl;
            //     if (!it1->autoindex.empty())
            //         std::cout << "autoindex : " << it1->autoindex<< std::endl;
            //     if (!it1->uploadfile.empty())
            //         std::cout << "uploadfile : " << it1->uploadfile<< std::endl;
            //     if (!it1->upload_location.empty())
            //         std::cout << "upload_location : " << it1->upload_location<< std::endl;
            //     if (!it1->cgi_allowed.empty())
            //         std::cout << "cgi_allowed : " << it1->cgi_allowed<< std::endl;
            //     // if (!it1->cgi_allowed.first.empty())
            //     //     std::cout << "cgi_allowed[0] : " << it1->cgi_allowed.first<< std::endl;
            //     std::cout << "------------------------end of location -------------------" << std::endl;

            // }
            // std::cout << "---------------------------------" << std::endl;
        }
        else
            throw std::runtime_error("Error : line have dosen't follow rules");
    }
}

ConfigFile::~ConfigFile()
{
    if (config.is_open())
        config.close();
}
