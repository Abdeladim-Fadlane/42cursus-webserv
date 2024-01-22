/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 18:56:09 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/21 14:27:17 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostMethod.hpp"
long PostMethod::i = 0;

PostMethod::PostMethod(const Requeste& r) : req(r)
{
    first_time = true;
    flag_boundary = false;
    buffer_add = r.getBody();
    separator_size = 0;
    size = 0;
    path = r.getPath();
    setFileextation("fileExtation", map_extation);
    content_type = this->req.requeste_map.find("Content-Type")->second;
    // content_type = map_extation.find(content_type)->second;
    content_length = atoi((this->req.requeste_map.find("Content-Length")->second).c_str());
    Transfer_Encoding = this->req.requeste_map.find("Transfer-Encoding")->second;
}

const std::string& PostMethod::getContentType(void) const 
{
    return (content_type);
}

size_t PostMethod::getContentLength(void) const 
{
    return (content_length);
}

int hexCharToDecimal(char hexChar) {
    if (std::isdigit(hexChar))
        return hexChar - '0';
    else
        return std::tolower(hexChar) - 'a' + 10;
}

int hexStringToDecimal(const std::string& hexString) {
    int decimalValue = 0;

    for (size_t i = 0; i < hexString.length(); ++i) 
        decimalValue = decimalValue * 16 + hexCharToDecimal(hexString[i]);
    return decimalValue;
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
    // std::cout << "nanoni" << std::endl;
    // std::cout <<content<< std::endl;
    return (map_extation.find(content)->second);
}


void PostMethod::boundary(std::string buffer)
{
    std::string line;
    size_t      index;
    
    buffer = buffer_add + buffer;
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
        boundary(buffer);
        buffer_add = boundary_separator + "--";
        Postfile.close();
    }
    else
    {
        index = buffer.find("-");
        separator_size = boundary_separator.length() - index;
        // separator_size = buffer.length() - index;
        if (buffer.find(boundary_separator) == 0)
        {
            if (Postfile.is_open())
                Postfile.close();
            content_type = init_contentType(buffer);
            gettimeofday(&Time, nullptr) ;
            Postfile.open(std::string(FILE).append("/index") + std::to_string(Time.tv_sec + Time.tv_usec) + content_type, std::fstream::out);
            boundary(buffer);
        }
        else if (buffer.find(boundary_separator) != std::string::npos)
        {
            Postfile << buffer.substr(0, buffer.find(boundary_separator));
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

void PostMethod::chunked(std::string &buffer)
{
    buffer = buffer_add + buffer;
    if (!size)
    {
        if (buffer.find("\r\n") != std::string::npos)
        {
            size = hexStringToDecimal(buffer.substr(0, buffer.find("\r\n")));
            // std::cout << buffer.substr(0, buffer.find("\r\n")) << std::endl;
            if (!size)
            {
                Postfile.close();
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
        chunked(buffer);
    }
}



void    PostMethod::PostingFileToServer(bool &flag)
{
    std::string buffer;
    char buffer_read[1024];
    int x;

    memset(buffer_read, 0, sizeof(buffer_read));
    x = read(req.getSocketFd(), buffer_read, 1023);
    if (x == 0)
        flag = true;
    buffer.append(buffer_read, x);
    if (!Transfer_Encoding.compare("chunked"))
    {
        if (first_time)
        {
            gettimeofday(&Time, nullptr);
            content_type = map_extation.find(content_type)->second; 
            Postfile.open(std::string(FILE) + std::to_string(Time.tv_sec + Time.tv_usec) + content_type, std::fstream::out);
        }
        first_time = false;
        chunked(buffer);
    }
    else if (!strncmp(this->req.requeste_map.find("Content-Type")->second.c_str(), "multipart/form-data", strlen("multipart/form-data")))
        boundary(buffer);
    else
    {
        if (first_time)
        {
            gettimeofday(&Time, NULL) ;
            content_type = map_extation.find(content_type)->second; 
            Postfile.open(std::string(FILE) + std::to_string(Time.tv_sec + Time.tv_usec) + content_type, std::fstream::out);
        }
        first_time = false;
        buffer = buffer_add + buffer;
        buffer_add = "";
        Postfile << buffer;
    }
}

PostMethod::~PostMethod()
{
    Postfile.close();
}
