/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 18:56:09 by akatfi            #+#    #+#             */
/*   Updated: 2024/02/04 16:08:03 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostMethod.hpp"

PostMethod::PostMethod(Requeste& r) : req(r)
{
    first_time = true;
    buffer_add = r.getBody();
    separator_size = 0;
    size = 0;
    content_file = 0;
    path = r.getPath();
    content_length = 0;
    setFileextation("akatfi/fileExtation", map_extation);
    if (req.requeste_map.find("Content-Type") != req.requeste_map.end())
        content_type = this->req.requeste_map.find("Content-Type")->second;
    if (req.requeste_map.find("Content-Length") != req.requeste_map.end())
        content_length = atoi((this->req.requeste_map.find("Content-Length")->second).c_str());
    if (req.requeste_map.find("Transfer-Encoding") != req.requeste_map.end())
        Transfer_Encoding = this->req.requeste_map.find("Transfer-Encoding")->second;
    req.headerResponse = "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\n\r\n";
    req.status_client = 201;
}

const std::string& PostMethod::getContentType(void) const 
{
    return (content_type);
}

size_t PostMethod::getContentLength(void) const 
{
    return (content_length);
}

std::string PostMethod::init_contentType(std::string& buffer)
{
    std::string line;
    std::string content;
    
    while (buffer.find("\r\n") != std::string::npos)
    {
        line = buffer.substr(0, buffer.find("\r\n"));
        buffer = buffer.substr(buffer.find("\r\n") + 2);
        if (line.empty())
            break ;
        if (line.find("Content-Type: ") != std::string::npos)
            content = line.substr(line.find("Content-Type: ") +  strlen("Content-Type: "));
    }
    if (map_extation.find(content) != map_extation.end())
        return (map_extation.find(content)->second);
    return ("");
}


void PostMethod::boundary(std::string buffer, bool& isdone)
{
    std::string line;
    size_t      index;
    
    buffer = buffer_add + buffer;
    content_file += buffer.length();
    buffer_add = "";
    if (first_time)
    {
        first_time = false;
        boundary_separator = this->req.requeste_map.find("Content-Type")->second;
        boundary_separator = std::string("--") + boundary_separator.substr(boundary_separator.find("boundary=") + strlen("boundary="));
    }
    if (buffer.find(boundary_separator + "--") != std::string::npos)
    {
        buffer = buffer.substr(0, buffer.find(boundary_separator + "--"));
        boundary(buffer, isdone);
        buffer_add = boundary_separator + "--";
        Postfile.close();
        isdone = true;
    }
    else
    {
        index = buffer.find("-");
        separator_size = boundary_separator.length() - index;
        if (buffer.find(boundary_separator) == 0)
        {
            if (Postfile.is_open())
                Postfile.close();
            content_type = init_contentType(buffer);
            if (content_type.empty())
            {
                req.status_client = 415;
                req.headerResponse = "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n";
                isdone = true;
            }
            gettimeofday(&Time, NULL) ;
            ss << Time.tv_sec << "-" << Time.tv_usec;
            vector_files.push_back(req.Location_Server.upload_location + "/index" + ss.str() + content_type);
            Postfile.open((std::string(req.Location_Server.upload_location).append("/index") + ss.str() + content_type).c_str() , std::fstream::out);
            ss.str("");
            boundary(buffer, isdone);
        }
        else if (buffer.find(boundary_separator) != std::string::npos)
        {
            Postfile << buffer.substr(0, buffer.find(boundary_separator) - 2);
            buffer = buffer.substr(buffer.find(boundary_separator));
            buffer_add = buffer;
        }
        else if (index != std::string::npos && index >= buffer.length() - separator_size)
        {
            Postfile << buffer.substr(0, index);
            buffer_add = buffer.substr(index + 1);
        }
        else
            Postfile << buffer;
    }
}

void PostMethod::chunked(std::string &buffer, bool& isdone)
{
    std::stringstream strhex;

    buffer = buffer_add + buffer;
    content_file += buffer.length();
    if (!size)
    {
        if (buffer.find("\r\n") != std::string::npos)
        {
            strhex << std::hex << buffer.substr(0, buffer.find("\r\n"));
            strhex >> size;
            strhex.str("");
            if (!size)
            {
                Postfile.close();
                isdone = true;
                return ;
            }
            buffer = buffer.substr(buffer.find("\r\n") + 2);
        }
        else
        {
            buffer_add = buffer;
            return ;
        }
    }
    if (size && size >= buffer.length())
    {
        Postfile << buffer;
        size -= buffer.length();
        buffer_add = "";
    }
    else if (size)
    {
        Postfile << buffer.substr(0, size);
        buffer = buffer.substr(buffer.find("\r\n") + 2);
        size = 0;
        buffer_add = "";
        chunked(buffer, isdone);
    }
}



void    PostMethod::PostingFileToServer(bool& isdone)
{
    std::string buffer;
    char buffer_read[1024];
    int x;

    // if (content_file >= req.Server_Requeste.max_body)
    // {
    //     unlink_all_file();
    //     req.status_client = 413;
    //     isdone = true;
    //     req.headerResponse = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\n\r\n";
    // }
    if (!req.Location_Server.uploadfile.compare("OFF") && !req.Location_Server.cgi_allowed.compare("OFF"))
    {
        req.headerResponse = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\n\r\n";
        req.status_client = 405;
        isdone = true;
        return ;
    }
    if (content_type.empty() || !content_length)
    {
        req.headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
        isdone = true;
        req.status_client = 400;
        return ;
    }
    memset(buffer_read, 0, sizeof(buffer_read));
    x = read(req.getSocketFd(), buffer_read, 1023);
    buffer.append(buffer_read, x);
    if ((buffer + req.getBody()).length() == 0)
    {
        req.headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
        isdone = true;
        req.status_client = 400;
        return ;
    }
    if (!Transfer_Encoding.compare("chunked"))
    {
        if (first_time)
        {
            gettimeofday(&Time, NULL);
            if (map_extation.find(content_type) != map_extation.end())
                content_type = map_extation.find(content_type)->second;
            else
                content_type = "";
            if (content_type.empty())
            {
                req.status_client = 415;
                req.headerResponse = "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n";
                isdone = true;
                return ;
            }
            ss << Time.tv_sec << "-" << Time.tv_usec;
            vector_files.push_back(req.Location_Server.upload_location + "/index" + ss.str() + content_type);
            Postfile.open((std::string(req.Location_Server.upload_location).append("/index")  + ss.str() + content_type).c_str(), std::fstream::out);
            ss.str("");
        }
        first_time = false;
        chunked(buffer, isdone);
    }
    else if (!strncmp(this->req.requeste_map.find("Content-Type")->second.c_str(), "multipart/form-data", strlen("multipart/form-data")))
        boundary(buffer, isdone);
    else
    {
        if (first_time)
        {
            gettimeofday(&Time, NULL) ;
            if (map_extation.find(content_type) != map_extation.end())
                content_type = map_extation.find(content_type)->second;
            else
                content_type = "";
            if (content_type.empty())
            {
                req.status_client = 415;
                req.headerResponse = "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n";
                isdone = true;
                return ;
            }
            ss << Time.tv_sec << "-" << Time.tv_usec;
            vector_files.push_back(req.Location_Server.upload_location + "/index" + ss.str() + content_type);
            content_type =std::string(req.Location_Server.upload_location).append("/index")  + ss.str() + content_type;
            Postfile.open(content_type.c_str(), std::fstream::out);
            ss.str("");
            first_time = false;
        }
        buffer = buffer_add + buffer;
        buffer_add = "";
        Postfile << buffer;
        content_file += buffer.length();
        if (content_length == content_file)
        {
            Postfile.close();
            isdone = true;
        }
    }
}

void PostMethod::unlink_all_file()
{
    for (unsigned int i = 0; i < vector_files.size(); i++)
        remove(vector_files[i].c_str());
}

PostMethod::~PostMethod()
{
    Postfile.close();
}
