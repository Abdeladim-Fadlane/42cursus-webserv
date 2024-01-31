/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requeste.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:13:01 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/31 20:03:25 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requeste.hpp"
#include "PostMethod.hpp"
Requeste::Requeste(int fd,ConfigFile &configfile) : config(configfile)
{
    fd_socket = fd;
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

void    Requeste::readFromSocketFd(bool &flag)
{
    char buffer[1024];
    int x;
    memset(buffer,0, sizeof(buffer));
    x = read(fd_socket, buffer, 1023);
    head.append(buffer, x);
    if (head.find("\r\n\r\n") != std::string::npos)
    {
        this->MakeMapOfHeader();
        this->get_infoConfig();
        flag = true;
        if (method == "POST" && !post)
            post = new PostMethod(*this);
    }
}

void Requeste::get_infoConfig()
{
    struct stat statbuf;

    for (std::vector<Server>::iterator it = config.Servers.begin() ; it != config.Servers.end(); it++)
    {
        if (it->host == this->host && it->listen == this->port)
        {
            for (unsigned int i = 0; i < it->locations.size(); i++)
            {
                if (!strncmp(it->locations[i].location_name.c_str(),path.c_str(), it->locations[i].location_name.length()))
                {
                    locationServer = it->locations[i];
                    locationServer.root += "/";
                    locationServer.root  += path.substr(it->locations[i].location_name.length());
                    stat(locationServer.root.c_str(), &statbuf);
                    if (path.length() && path[path.length() - 1] != '/' && S_ISDIR(statbuf.st_mode) == true)
                        path += "/";
                    break ;
                }
                else if (path == "/" && it->locations.size() - 1 == i)
                {
                    path = it->locations[0].location_name;
                    locationServer = it->locations[0];
                    break;
                }
            }
            break ;
        }
    }
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