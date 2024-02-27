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
    done = false;
    chose_location = false;
    skeeptime_out = false;
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
    memset(buffer, 0, sizeof(buffer));

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
    else
        headerResponse = "";
    if (status_client != 0)
    {
        if (read(fdresponse, buffer, 1023) == 0)
        {
            readyclose  = true;
            return ;
        }
    }
    if (write(fd_socket, headerResponse.append(buffer).c_str(), headerResponse.length()) == -1)
    {
        readyclose  = true;
        return ;
    }       
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
        {
            isdone = true;
            skeeptime_out = true;
        }
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
    size_t pos;
    std::stringstream str;
    size_t p;
    char new_value;

    while (path.size() > 0 && path[0] == '/')
        path = path.substr(1);
    while (path.size() > 0  && path[path.size() - 1] == '/')
    {
        path = path.substr(0, path.size() - 1);
        slash = true;
    }
    if (slash == true)
        path += "/";
    path = "/" + path;
    while (path.find("%") != std::string::npos)
    {
        pos = path.find("%");
        str << std::hex << path.substr(pos + 1 , 2);
        str >> p;
        new_value = static_cast<char>(p);
        path.replace(pos, 3, std::string("") += new_value);
        str.str("");
    }
    return (path);
}

bool check_string(const std::string&  str1 , std::string  str2)
{
    for (size_t i = 0; i <= str1.length() ; i++)
    {
        if (str2.find(str1[i]) != std::string::npos)
            return true;
    }
    return (false);
}

void Requeste::get_infoConfig(bool& isdone)
{
    struct stat statbuf;
    bool slash = false;
    size_t length;
    char *pathreal = NULL;
    std::stringstream ss;
    std::string root_path;

    path = delete_slash_path(path, slash);
    if (check_string(path, "{}|\\^~[]`"))
    {
        status_client = 400;
        isdone = true;
        method = "";
        headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
        return ;
    }
    for (unsigned int i = 0; i < Server_Requeste.locations.size(); i++)
    {
        if (!strncmp(Server_Requeste.locations[i].location_name.c_str(),path.c_str(), Server_Requeste.locations[i].location_name.length()))
        {
            chose_location = true;
            Location_Server = Server_Requeste.locations[i];
            root_path = Location_Server.root;
            length = Server_Requeste.locations[i].location_name.length();
            if (length > 1 && length + 1 <= path.length())
                length += 1;
            Location_Server.root  += path.substr(length);
            if (stat(Location_Server.root.c_str(), &statbuf) == 0 && 
                S_ISDIR(statbuf.st_mode) == true &&  path.length() && path[path.length() - 1] != '/' && method == "GET")
            {
                status_client = 0;
                isdone = true;
                method =  "";
                ss << Server_Requeste.listen;
                headerResponse = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://" + Server_Requeste.host.append(":") 
                    + ss.str() + path.append("/") + "\r\n\r\n";
                ss.str("");
            }
            if (Server_Requeste.locations[i].location_name == "/")
            {
                continue;
            }
            else
                break;
        }
        else if (path == "/" && Server_Requeste.locations.size() - 1 == i && chose_location == false)
        {
            path = Server_Requeste.locations[0].location_name;
            Location_Server = Server_Requeste.locations[0];
            root_path = Location_Server.root;
            chose_location = true;
            if (path.length() && path[path.length() - 1] != '/' && method == "GET")
            {
                status_client = 0;
                isdone = true;
                method =  "";
                ss << Server_Requeste.listen;
                headerResponse = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://" + Server_Requeste.host.append(":") 
                    + ss.str() + path.append("/") + "\r\nContent-Type: text/html\r\n\r\n";
                ss.str("");
                return ;
            }
            break;
        }
    }
    if (Location_Server.upload_location.empty())
        Location_Server.upload_location = Location_Server.root;
    if (Location_Server.redirection.empty() == false)
    {
        status_client = 0;
        isdone = true;
        method =  "";
        ss << Server_Requeste.listen;
        headerResponse = "HTTP/1.1 301 Moved Permanently\r\nLocation: " + Location_Server.redirection + "\r\n\r\n";
        ss.str("");
        return ;
    }
    pathreal = realpath(Location_Server.root.c_str(), NULL);
    if (pathreal && std::string(pathreal).append("/").find(root_path.c_str()) == std::string::npos)
    {
        method = "";
        headerResponse = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n";
        status_client = 403;
        isdone = true;
    }
    if (pathreal)
        free(pathreal);
}

void Requeste::MakeMapOfHeader(bool& isdone)
{
    std::string     new_req;
    std::string     line;
    std::vector<std::string> line_request;
    std::vector<Server> s;
    

    body = head.substr(head.find("\r\n\r\n") + 4);
    head = head.substr(0, head.find("\r\n\r\n"));
    if (head.empty() == false)
    {
        line_request = split_line(head.substr(0, head.find("\r\n")));
        method = line_request[0];
        path = line_request[1];
        http_v = line_request[2];
        if (head.find("\r\n") != std::string::npos)
            head = head.substr(head.find("\r\n") + 2).append("\r\n");
    }
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
            for (size_t j = 0; j < config.Servers.size(); j++)
                if (config.Servers[j].listen == port)
                    s.push_back(config.Servers[j]);
            for (size_t i = 0; i < s.size(); i++)
            {
                if (s[i].host == host)
                {
                    Server_Requeste = s[i];
                    break ;
                }
                if (s[i].server_name == host)
                {
                    Server_Requeste = s[i];
                    break ;
                }
                if (i + 1 == s.size() && host.empty() == false)
                {
                    Server_Requeste = s[0];
                    break ;
                }
            }
        }
    }
    if (host.empty() || !port)
    {
        status_client = 400;
        isdone = true;
        method = "";
        headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
        return ;
    }
    if (method != "GET" && method != "POST" && method != "DELETE")
    {
        headerResponse = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n";
        isdone = true;
        status_client = 501;
        return ;
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