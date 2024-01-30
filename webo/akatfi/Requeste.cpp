/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requeste.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:13:01 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/29 18:06:58 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requeste.hpp"
#include "PostMethod.hpp"
Requeste::Requeste(int fd)
{
    fd_socket = fd;
    RunMethod = false;
    post = NULL;
}

std::pair<std::string, std::string> Requeste::MakePair(std::string& line)
{
    std::string first;
    int         i;

    i = 0;
    first = line.substr(0, line.find(":"));
    line = line.substr(line.find(":") + 1);
    while (isspace(line[i]))
        line = line.substr(1);
    return (std::pair<std::string, std::string>(first, line));
}

void    Requeste::readFromSocketFd(bool &flag,int fd)
{
    char buffer[1024];
    int x;
    (void)fd;
    memset(buffer,0, sizeof(buffer));
    x = read(fd_socket, buffer, 1023);
    head.append(buffer, x);
    if (head.find("\r\n\r\n") != std::string::npos)
    {
        this->MakeMapOfHeader();
        this->get_infoConfig();
        flag = 1;
        if (method == "POST" && !post)
            post = new PostMethod(*this);
    }
}

void Requeste::get_infoConfig()
{
    ConfigFile config("/nfs/homes/akatfi/Desktop/webserv/akatfi/parsinfCon/file.config");
    
    config.parceConfig();

    for (std::vector<Server>::iterator it = config.Servers.begin() ; it != config.Servers.end(); it++)
    {
        if (it->host == this->host && it->listen == this->port)
        {
            for (unsigned int i = 0; i < it->locations.size(); i++)
            {
                if (it->locations[i].location_name == path)
                {
                    // path += it->locations[i].root;
                    locationServer = it->locations[i];
                    break ;
                }
                else if (i + 1 == it->locations.size())
                {
                    // path = it->locations[0].location_name + it->locations[0].root;
                    locationServer = it->locations[0];
                    break ;
                }
            }
            break ;
        }
        if (it + 1 == config.Servers.end())
        {
            // Error message
        }
    }
    std::cout << locationServer.root << std::endl;
}

void Requeste::MakeMapOfHeader()
{
    std::string     new_req;
    std::string     line;
    
    body = head.substr(head.find("\r\n\r\n") + 4);
    head = head.substr(0, head.find("\r\n\r\n"));
    method = head.substr(0, head.find(" "));
    head = head.substr(head.find(" ") + 1);
    path = head.substr(0, head.find(" "));
    head = head.substr(head.find(" ") + 1);
    http_v = head.substr(0, head.find("\r\n"));
    head = head.substr(head.find("\r\n") + 2).append("\r\n");
    while (head.length())
    {
        line = head.substr(0, head.find("\r\n"));
        head = head.substr(head.find("\r\n") + 2);
        requeste_map.insert(MakePair(line));
    }
    if (http_v != "HTTP/1.1")
        throw std::runtime_error("400");
    if (path.length() > 2048)
        throw std::runtime_error("400");
    host = requeste_map.find("Host")->second;
    port = atoi(host.substr(host.find(":") + 1).c_str());
    host = host.substr(0, host.find(":"));
}

int Requeste::getSocketFd() const
{
    return (this->fd_socket);
}

const std::string& Requeste::getBody() const
{
    return (this->body);
}

const std::string& Requeste::getPath() const
{
    return (this->path);
}

Requeste::~Requeste()
{
    close(fd_socket);
    delete post;
}

void setFileextation(std::string const& f, std::map<std::string, std::string>& Extation)
{
    std::fstream                            file;
    std::string                             input;
    std::string                             first;

    file.open(f.c_str());
    while (std::getline(file, input))
    {
        first = input.substr(0, input.find(":"));
        input = input.substr(input.find(":") + 1);
        Extation[first] = input;
    }
    file.close();
}