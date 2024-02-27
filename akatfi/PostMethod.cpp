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
    isCgi = false;
    buffer_add = r.getBody();
    size = 0;
    content_file = 0;
    path = r.getPath();
    content_length = 0;
    setFileextation("akatfi/fileExtation", map_extation);
    if (req.requeste_map.find("Content-Type") != req.requeste_map.end())
        content_type = this->req.requeste_map.find("Content-Type")->second;
    if (req.requeste_map.find("Content-Length") != req.requeste_map.end())
    {
        if (check_digit(req.requeste_map.find("Content-Length")->second) == false)
            content_length  = -1;
        else
            content_length = atoi((this->req.requeste_map.find("Content-Length")->second).c_str());
    }
    if (req.requeste_map.find("Transfer-Encoding") != req.requeste_map.end())
        Transfer_Encoding = this->req.requeste_map.find("Transfer-Encoding")->second;
    req.headerResponse = "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\n\r\n";
    req.status_client = 201;
    if (req.Location_Server.cgi_allowed == "ON")
    {
        cgi_file.open("/tmp/index_cgi", std::fstream::out);
        cgi_path = "/tmp/index_cgi";
        ft_prepar_cgi();
    }
}

void    PostMethod::ft_prepar_cgi()
{
    struct stat stat_buffer;
    DIR *dir;
    struct dirent* dirent;
    std::vector<std::string> scripts;

    if (stat(req.Location_Server.root.c_str(), &stat_buffer) == 0 && S_ISDIR(stat_buffer.st_mode))
    {
        for (std::vector<std::string>::iterator it = req.Location_Server.indexs.begin(); it != req.Location_Server.indexs.end(); it++)
            if (it->find(".php") != std::string::npos || it->find(".py")  != std::string::npos || it->find(".pl")  != std::string::npos)
                scripts.push_back(*it);
        dir = opendir(req.Location_Server.root.c_str());
        while ((dirent = readdir(dir)) != NULL)
        {
            if (std::find(scripts.begin(),scripts.end(), dirent->d_name) != scripts.end() &&
                access((req.Location_Server.root + dirent->d_name).c_str(), X_OK) == 0)
            {
                script_path = req.Location_Server.root;
                script_path.append(dirent->d_name);
                cgi_extation = script_path.substr(script_path.rfind("."));
                isCgi = true;
                break ;
            }
        }
        closedir(dir);
        if (script_path.empty())
        {
            req.Location_Server.cgi_allowed = "OFF";
            return ;
        }
    }
    else
    {
        if (access(req.Location_Server.root.c_str(), X_OK) == -1)
        {
            req.Location_Server.cgi_allowed = "OFF";
            return ;
        }
        if (req.Location_Server.root.find(".php") != std::string::npos || req.Location_Server.root.find(".py") != std::string::npos 
            || req.Location_Server.root.find(".pl") != std::string::npos)
        {
            isCgi = true;
            script_path = req.Location_Server.root;
            cgi_extation = script_path.substr(script_path.rfind("."));
        }
    }
    if (script_path.empty())
        req.Location_Server.cgi_allowed = "OFF";
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
    
    while (1)
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
    size_t index;
    std::string type;

    buffer =  buffer_add + buffer;
    buffer_add = "";
    if (first_time)
    {
        first_time = false;
        boundary_separator = this->req.requeste_map.find("Content-Type")->second;
        if (boundary_separator.find("boundary=") != std::string::npos)
            boundary_separator = std::string("--") + boundary_separator.substr(boundary_separator.find("boundary=") + strlen("boundary="));
        else 
           boundary_separator = ""; 
        if (boundary_separator.empty())
        {
            req.status_client = 400;
            req.headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
            isdone = true;
            return ;
        }
        buffer_add_size = (boundary_separator + "\r\n").length();
    }
    if (isCgi == true)
    {
        cgi_file << buffer;
        if (buffer.find(boundary_separator + "--\r\n") != std::string::npos)
            isdone = true;
        return ;
    }
    if (buffer.find(boundary_separator + "--\r\n") != std::string::npos)
    {
        buffer = buffer.substr(0, buffer.find(boundary_separator + "--\r\n") - 2);
        buffer_add_size = 0;
        boundary(buffer, isdone);
        if (Postfile.is_open())
            Postfile.close();
        req.skeeptime_out = true;
        isdone = true;
    }
    else if (buffer.find(boundary_separator + "\r\n") != std::string::npos)
    {
        if (Postfile.is_open() == true)
        {
            Postfile << buffer.substr(0, buffer.find(boundary_separator + "\r\n") - 2);
            content_file += buffer.substr(0, buffer.find(boundary_separator + "\r\n") - 2).length();
            Postfile.close();
            buffer = buffer.substr(buffer.find(boundary_separator + "\r\n"));
        }
        if (buffer.find("\r\n\r\n") == std::string::npos)
        {
            buffer_add = buffer;
            return ;
        }
        type = init_contentType(buffer);
        if (type.empty() == true)
        {
            req.status_client = 415;
            req.headerResponse = "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n";
            isdone = true;
            return ;
        }
        gettimeofday(&Time, NULL);
        ss << Time.tv_sec << "-" << Time.tv_usec;
        vector_files.push_back(req.Location_Server.upload_location + "/index" + ss.str() + type);
        Postfile.open((std::string(req.Location_Server.upload_location).append("/index") + ss.str() + type).c_str() , std::fstream::out);
        ss.str("");
        boundary(buffer, isdone);
    }
    else
    {
        if (buffer.length() > buffer_add_size)
            index = buffer.length() - buffer_add_size;
        else
            index = buffer.length();
        Postfile << buffer.substr(0, index);
        content_file += buffer.substr(0, index).length();
        buffer_add = buffer.substr(index);
    }
}

size_t hexStringToDecimal(const std::string hexString) {
    size_t decimalValue = 0;
    std::stringstream ss;

    ss << std::hex << hexString;
    ss >> decimalValue ;
    return decimalValue;
}

void PostMethod::chunked(std::string buffer, bool& isdone)
{
    std::string hex;
    buffer = buffer_add + buffer;
    buffer_add = "";
  
    if (!size)
    {
        if (buffer.find("\r\n") != std::string::npos)
        {
            hex = buffer.substr(0, buffer.find("\r\n"));
            size = hexStringToDecimal(hex);
            if (size == 0)
            {
                Postfile.close();
                cgi_file.close();
                isdone = true;
                req.skeeptime_out = true;
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
    if (buffer.length() != 0)
    {
        if (size && size >= buffer.length())
        {
            if (isCgi == false)
                Postfile << buffer;
            else
                cgi_file << buffer;
            size -= buffer.length();
            content_file += buffer.length();
        }
        else if (size)
        {
            if (isCgi == false)
                Postfile << buffer.substr(0, size);
            else
                cgi_file << buffer.substr(0, size);
            content_file += buffer.substr(0, size).length();
            buffer = buffer.substr(size + 2);
            size = 0;
            chunked(buffer, isdone);
        }
    }
}

void    PostMethod::PostingFileToServer(bool& isdone, bool readorno)
{
    std::string buffer;
    char buffer_read[1024];
    int x;

    if (access(req.Location_Server.root.c_str(), F_OK) == - 1)
    {
        req.status_client = 404;
        isdone = true;
        req.headerResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        return ;
    }
    if (req.requeste_map.find("Content-Length") == req.requeste_map.end() && Transfer_Encoding.empty())
    {
        req.headerResponse = "HTTP/1.1 411 Length Required\r\nContent-Type: text/html\r\n\r\n";
        req.status_client = 411;
        isdone = true;
        return ;
    }
    if (content_length < 0 || content_type.empty() || (content_file > content_length && Transfer_Encoding != "chunked"))
    {
        Postfile.close();
        unlink_all_file();
        req.headerResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
        req.status_client = 400;
        isdone = true;
        return ;
    }
    if (req.Location_Server.uploadfile == "OFF" && req.Location_Server.cgi_allowed == "OFF")
    {
        req.headerResponse = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n";
        req.status_client = 403;
        isdone = true;
        return ;
    }
    if ((Transfer_Encoding == "chunked" && content_type.substr(0, content_type.find(";")) == "multipart/form-data")
        || (Transfer_Encoding.empty() == false && Transfer_Encoding != "chunked"))
    {
        req.headerResponse = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n";
        isdone = true;
        req.status_client = 501;
        return ;
    }
    if (readorno == true)
    {
        memset(buffer_read, 0, sizeof(buffer_read));
        if ((x = read(req.getSocketFd(), buffer_read, 1023)) == -1)
        {
            req.status_client = 500;
            isdone = true;
            req.headerResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
            return ;
        }
        buffer.append(buffer_read, x);
    }
    if ((buffer + buffer_add).length() == 0)
    {
        if (Postfile.is_open())
            Postfile.close();
        if (cgi_file.is_open())
            cgi_file.close();
        isdone = true;
        return ;
    }
    if (Transfer_Encoding == "chunked")
    {
        if (first_time && isCgi == false)
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
    else if (content_type.substr(0, content_type.find(";")) == "multipart/form-data")
        boundary(buffer, isdone);
    else
    {
        if (first_time && isCgi == false)
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
            content_type = std::string(req.Location_Server.upload_location).append("/index")  + ss.str() + content_type;
            Postfile.open(content_type.c_str(), std::fstream::out);
            ss.str("");
            first_time = false;
        }
        buffer = buffer_add + buffer;
        buffer_add = "";
        content_file += buffer.length();
        if (isCgi == false)
            Postfile << buffer;
        else
            cgi_file << buffer;
        if (content_length == content_file)
        {
            if (Postfile.is_open())
                Postfile.close();
            if (cgi_file.is_open())
                cgi_file.close();
            req.skeeptime_out = true;
            isdone = true;
        }
    }
    if (Postfile.fail() || cgi_file.fail())
    {
        Postfile.close();
        cgi_file.close();
        if (access(cgi_path.c_str(), F_OK) != -1)
            remove(cgi_path.c_str());
        unlink_all_file();
        req.status_client = 500;
        isdone = true;
        req.headerResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
    }
    if (content_file > req.Server_Requeste.max_body)
    {
        if (Postfile.is_open())
            Postfile.close();
        unlink_all_file();
        req.status_client = 413;
        isdone = true;
        req.headerResponse = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\n\r\n";
    }
}

void PostMethod::unlink_all_file()
{
    if (Postfile.is_open())
        Postfile.close();
    for (unsigned int i = 0; i < vector_files.size(); i++)
        remove(vector_files[i].c_str());
}

PostMethod::~PostMethod()
{
    if (cgi_file.is_open())
        cgi_file.close();
    if (Postfile.is_open())
        Postfile.close();
    if (req.Location_Server.uploadfile == "OFF" || req.done == true)
        unlink_all_file();
}
