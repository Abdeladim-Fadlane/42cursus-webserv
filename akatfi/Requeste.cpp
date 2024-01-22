/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requeste.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:13:01 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/20 12:22:25 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requeste.hpp"
#include "PostMethod.hpp"
Requeste::Requeste(int fd):fd_socket(fd)
{
    // fd_socket = fd;
    std::cout << fd_socket<< std::endl;
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

void    Requeste::readFromSocketFd(bool &flag)
{
    char buffer[1024];
    int x;

    memset(buffer,0, sizeof(buffer));
    // std::cout <<  "====== "<< fd_socket << std::endl;
    x = read(fd_socket, buffer, 1023);
    head += std::string("").append(buffer, x);
    if (head.find("\r\n\r\n") != std::string::npos)
    {
        this->MakeMapOfHeader();
        flag = 1;
        if (method == "POST" && !post)
            post = new PostMethod(*this);
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
    
    // host = requeste_map.find("Host")->second;
    // port = atoi(host.substr(host.find(":") + 1).c_str());
    // host = host.substr(0, host.find(":"));
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
    // for (std::map<std::string, std::string>::iterator it = requeste_map.begin(); it != requeste_map.end(); it++)
    //     std::cout << it->first << " | " << it->second << std::endl;
    close(fd_socket);
    delete post;
}

void setFileextation(std::string const& f, std::map<std::string, std::string>& Extation)
{
    std::fstream                            file;
    std::string                             input;
    std::string                             first;
    // std::map<std::string, std::string>      Extation;

    file.open(f.c_str());
    while (std::getline(file, input))
    {
        first = input.substr(0, input.find(":"));
        input = input.substr(input.find(":") + 1);
        Extation[first] = input;
    }
    file.close();
}