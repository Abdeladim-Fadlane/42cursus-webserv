/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requeste.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:13:01 by akatfi            #+#    #+#             */
/*   Updated: 2024/02/04 16:03:06 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requeste.hpp"
#include "PostMethod.hpp"
Requeste::Requeste(int fd,ConfigFile &configfile) : config(configfile)
{
    fd_socket = fd;
    fdresponse = -1;
    post = NULL;
    port = 0;
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

void    Requeste::set_status_client(bool &readyclose)
{
    char buffer[1024];

    if (fdresponse == -1)
    {
        file_name = Server_Requeste.error_pages[status_client];
        fdresponse = open(file_name.c_str(), O_RDONLY);
    }
    else 
        headerResponse = "";
    memset(buffer, 0, sizeof(buffer));
    if (!read(fdresponse, buffer, 1023))
    {
        close(fdresponse); 
        readyclose = true;
    }
    else
        write(fd_socket, headerResponse.append(buffer).c_str(), headerResponse.length());  
}

void    Requeste::readFromSocketFd(bool &isdone, bool &flag)
{
    char buffer[1024];
    int x;
    memset(buffer,0, sizeof(buffer));
    x = read(fd_socket, buffer, 1023);
    head.append(buffer, x);
    if (head.find("\r\n\r\n") != std::string::npos)
    {
        this->MakeMapOfHeader(isdone);
        this->get_infoConfig(isdone);
        flag = true;
        if (isdone == false && std::find(Location_Server.allowed_method.begin(), Location_Server.allowed_method.end(), method) == Location_Server.allowed_method.end())
        {
            status_client = 405;
            isdone = true;
            method = "";
            headerResponse = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\n\r\n";
        }
        else if  (isdone == false && method == "POST" && !post)
            post = new PostMethod(*this);
        else if (isdone == false)
            isdone = true;
    }
    else if (head.length() >= 4000)
    {
        status_client = 413;
        isdone = true;
        headerResponse = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\n\r\n";
    }
    else if (head.length() == 0 && flag == false)
    {
        status_client = 404;
        isdone = true;
        headerResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
    }
}

std::string delete_slash_path(std::string& path, bool& slash)
{
    while (path.size() > 1 && path[0] == '/')
        path = path.substr(1);
    while (path.size() > 0  && path[path.size() - 1] == '/')
    {
        path = path.substr(0, path.size() - 1);
        slash = true;
    }
    return (path);
}

void Requeste::get_infoConfig(bool& isdone)
{
    struct stat statbuf;
    bool slash = false;

    path = "/" + delete_slash_path(path, slash);
    for (std::vector<Server>::iterator it = config.Servers.begin() ; it != config.Servers.end(); it++)
    {
        if (it->host == this->host && it->listen == this->port)
        {
            Server_Requeste = *it;
            for (unsigned int i = 0; i < it->locations.size(); i++)
            {
                if (!strncmp(it->locations[i].location_name.c_str(),path.c_str(), it->locations[i].location_name.length()))
                {
                    Location_Server = it->locations[i];
                    Location_Server.root += "/";
                    if (stat((Location_Server.root + Location_Server.upload_location).c_str(), &statbuf) != 0)
                        Location_Server.upload_location = "";
                    Location_Server.upload_location = Location_Server.root + Location_Server.upload_location;
                    Location_Server.root  += path.substr(it->locations[i].location_name.length());
                    stat(Location_Server.root.c_str(), &statbuf);
                    if (path.length() && path[path.length() - 1] != '/' && S_ISDIR(statbuf.st_mode) == true)
                        path += "/";
                    else if (S_ISREG(statbuf.st_mode) && slash == true)
                    {
                        status_client = 404;
                        isdone = true;
                        method =  "";
                        headerResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
                        return ;
                    }
                    break ;
                }
                else if (path == "/" && it->locations.size() - 1 == i)
                {
                    path = it->locations[0].location_name;
                    Location_Server = it->locations[0];
                    if (stat((Location_Server.root + Location_Server.upload_location).c_str(), &statbuf) != 0)
                        Location_Server.upload_location = "";
                    Location_Server.upload_location = Location_Server.root + Location_Server.upload_location;
                    break;
                }
            }
            break ;
        }
    }
}

void Requeste::MakeMapOfHeader(bool& isdone)
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
    {
        status_client = 505;
        isdone = true;
        method = "";
        headerResponse = "HTTP/1.1 505 HTTP Version Not Supported\r\nContent-Type: text/html\r\n\r\n";

    }
    if (path.length() > 2048)
    {
        status_client = 414;
        isdone = true;
        method =  "";
        headerResponse = "HTTP/1.1 414 URI Too Long\r\nContent-Type: text/html\r\n\r\n";
    }
    if (path.find("?") != std::string::npos)
    {
        query_str = path.substr(path.find("?") + 1);
        path = path.substr(0, path.rfind("?"));
    }
    if (requeste_map.find("Content-Type") != requeste_map.end())
        content_type = requeste_map.find("Content-Type")->second;
    if (requeste_map.find("Content-Length") != requeste_map.end())
        content_length = requeste_map.find("Content-Length")->second;
    if (requeste_map.find("Host") != requeste_map.end())
    {
        host = requeste_map.find("Host")->second;
        if (host.find(":") != std::string::npos)
        {
            port = atoi(host.substr(host.find(":") + 1).c_str());
            host = host.substr(0, host.find(":"));
        }
    }
    if (host.empty() || !port)
    {
        status_client = 400;
        isdone = true;
        method = "";
        headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
    }
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