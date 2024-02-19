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
    chose_location = false;
    port = 0;
    time_out = get_time();
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
        if (status_client == 0)
            readyclose  = true;
        else
        {
            file_name = Server_Requeste.error_pages[status_client];
            fdresponse = open(file_name.c_str(), O_RDONLY);
        }
    }
    else if (status_client != 0)
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

long    Requeste::get_time(void)
{
    struct timeval time;

    gettimeofday(&time, NULL);
    return (time.tv_sec + (time.tv_usec / 1000000));
}

void    Requeste::readFromSocketFd(bool &isdone, bool &flag)
{
    char buffer[1024];
    int x;
    memset(buffer,0, sizeof(buffer));
    if (get_time() - time_out > 30)
    {
        status_client = 408;
        isdone = true;
        headerResponse = "HTTP/1.1 408 Request Timeout\r\nContent-Type: text/html\r\n\r\n";
        return ;
    }
    if ((x = read(fd_socket, buffer, 1023)) == -1)
    {
        status_client = 500;
        isdone = true;
        headerResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
        return ;
    }
    head.append(buffer, x);
    if (head.find("\r\n\r\n") != std::string::npos)
    {
        // std::cout <<"\t\t -------------------------- \t\t\n" << head <<"\t\t -------------------------- \t\t" << std::endl;
        if (head.length() >= 4000)
        {
            status_client = 413;
            isdone = true;
            headerResponse = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\n\r\n";
            return ;
        }
        this->MakeMapOfHeader(isdone);
        this->get_infoConfig(isdone);
        flag = true;
        if (isdone == false && chose_location && std::find(Location_Server.allowed_method.begin(), Location_Server.allowed_method.end(), method) == Location_Server.allowed_method.end())
        {
            status_client = 405;
            isdone = true;
            method = "";
            headerResponse = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\n\r\n";
        }
        else if  (isdone == false && method == "POST" && !post)
        {
            post = new PostMethod(*this);
            post->PostingFileToServer(isdone, false);
        }
        else if (isdone == false)
            isdone = true;
    }
    else if (head.length() == 0 && flag == false)
    {
        status_client = 400;
        isdone = true;
        headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
    }
}

std::string delete_slash_path(std::string& path, bool& slash)
{
    unsigned int i = 0;
    while (path.size() > 0 && path[0] == '/')
        path = path.substr(1);
    while (path.size() > 0  && path[path.size() - 1] == '/')
    {
        path = path.substr(0, path.size() - 1);
        slash = true;
    }
    if (slash == true)
        path += "/";
    return ( "/" + path);
}



void Requeste::get_infoConfig(bool& isdone)
{
    struct stat statbuf;
    bool slash = false;
    bool flag = false;
    size_t length;

    path = delete_slash_path(path, slash);
    for (unsigned int i = 0; i < Server_Requeste.locations.size(); i++)
    {
        if (!strncmp(Server_Requeste.locations[i].location_name.c_str(),path.c_str(), Server_Requeste.locations[i].location_name.length()))
        {
            chose_location = true;
            Location_Server = Server_Requeste.locations[i];
            length = Server_Requeste.locations[i].location_name.length();
            if (length > 1 && length + 1 <= path.length())
                length += 1;
            Location_Server.root  += path.substr(length);
            if (stat(Location_Server.root.c_str(), &statbuf) == 0 && 
                S_ISDIR(statbuf.st_mode) == true &&  path.length() && path[path.length() - 1] != '/' && method != "DELETE")
            {
                status_client = 0;
                isdone = true;
                method =  "";
                headerResponse = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://" + Server_Requeste.host.append(":") 
                    + std::to_string(Server_Requeste.listen) + path.append("/") + "\r\n\r\n";
            }
            flag = true;
            if (Server_Requeste.locations[i].location_name == "/")
                continue;
            else
                break ;
        }
        else if (path == "/" && Server_Requeste.locations.size() - 1 == i && flag == false)
        {
            path = Server_Requeste.locations[0].location_name;
            Location_Server = Server_Requeste.locations[0];
            chose_location = true;
            if (path.length() && path[path.length() - 1] != '/' && method != "DELETE")
            {
                status_client = 0;
                isdone = true;
                method =  "";
                headerResponse = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://" + Server_Requeste.host.append(":") 
                    + std::to_string(Server_Requeste.listen) + path.append("/") + "\r\n\r\n";
            }
            break;
        }
    }
    if (Location_Server.upload_location.empty())
        Location_Server.upload_location = Location_Server.root;
    if (Location_Server.redirection.empty() == false)
    {
        // std::cout << "redirction with success !" << std::endl;
        status_client = 0;
        isdone = true;
        method =  "";
        headerResponse = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://" + Server_Requeste.host.append(":") 
            + std::to_string(Server_Requeste.listen) + Location_Server.redirection + "\r\n\r\n";
    }
}

void Requeste::MakeMapOfHeader(bool& isdone)
{
    std::string     new_req;
    std::string     line;
    std::vector<std::string> line_request;
    
    body = head.substr(head.find("\r\n\r\n") + 4);
    head = head.substr(0, head.find("\r\n\r\n"));
    line_request = split_line(head.substr(0, head.find("\r\n")));
    method = line_request[0];
    path = line_request[1];
    http_v = line_request[2];
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
        query_str = path.substr(path.rfind("?") + 1);
        path = path.substr(0, path.rfind("?"));
        std::cout << query_str << std::endl;
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
            for (size_t i = 0; i < config.Servers.size(); i++)
                if (config.Servers[i].host == host && config.Servers[i].listen == port)
                {
                    Server_Requeste = config.Servers[i];
                    break ;
                }
        }
        else
        {
            for (size_t i = 0; i < config.Servers.size(); i++)
                if (host == config.Servers[i].server_name)
                {
                    Server_Requeste = config.Servers[i];
                    return ;
                }
        }
    }
    if (host.empty() || !port)
    {
        status_client = 400;
        isdone = true;
        method = "";
        headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
    }
    // std::cout << "request :: " << content_type << content_length << std::endl;
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