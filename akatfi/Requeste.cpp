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
Requeste::Requeste(int fd,ConfigFile &configfile, bool &f) : config(configfile), flag(f)
{
    fd_socket = fd;
    fdresponse = -6;
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

bool    Requeste::set_status_client()
{
    char buffer[1024];
    
    if (fdresponse == -6)
    {
        std::string file_name = Server_Requeste.error_pages[status_client];
        fdresponse = open(file_name.c_str(), O_RDONLY);
    }
    memset(buffer, 0, sizeof(buffer));
    if (!read(fdresponse, buffer, 1023))
        return close(fdresponse),false;
    write(fd_socket, buffer, strlen(buffer));
    return true;
}

void    Requeste::readFromSocketFd(bool &flag, bool& isdone)
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
        if (std::find(Location_Server.allowed_method.begin(), Location_Server.allowed_method.end(), method) == Location_Server.allowed_method.end())
        {
            //take permission to write an error page;
            std::cout << "method is not allowed" << std::endl;
        }
        if (method == "POST" && !post)
            post = new PostMethod(*this);
        else
        {
            isdone = true;
        }
    }
}

void Requeste::get_infoConfig()
{
    struct stat statbuf;

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
    if (stat(Location_Server.root.c_str(), &statbuf) != 0)
    {
        //write error in socket 
        std::cout << "path not found !" << std::endl;
    }
    // std::cout << "uplaod path --> " << Location_Server.upload_location << std::endl;
    // std::cout << "server path --> " << Location_Server.root << std::endl;
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
    {
        // error of http version;
        std::cout << "the server not sepport this version of http !" << std::endl;
    }
    if (path.length() > 2048)
    {
        // write error in socket 
        std::cout << "path length is over" << std::endl;
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
        // error the requste not has a port or host
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