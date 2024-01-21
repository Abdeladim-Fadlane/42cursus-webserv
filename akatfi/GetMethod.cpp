/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetMethod.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 15:49:51 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/09 17:10:18 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetMethod.hpp"
#include <fcntl.h> 

GetMethod::GetMethod(const Requeste& req) : r (req)
{
    path = r.getPath();
    fd_socket = r.getSocketFd();
    first_time = false;
    list_dir = false;
}

void GetMethod::PreparHeader()
{   
    header = "HTTP/1.1 200 OK\r\n";
    header = header + std::string("Content-Type: ") + content_type + "\r\n";
    header = header + std::string("Content-Length: ") + std::to_string(content_length) + "\r\n\r\n";
}

void GetMethod::headerOfFile(void)
{
    std::map<std::string, std::string> map;
    std::fstream GetFile;
    
    map = setFileextation("fileExtation");
    Getfile.open(path.c_str());
    if (Getfile.is_open() == false)
        throw std::runtime_error("400");
    content_type = path.substr(path.rfind(".") + 1);
    for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++)
    {
        if (!it->second.compare(content_type))
        {
            content_type = it->first;
            break ;
        }
    }
    if (content_type.empty())
        throw std::runtime_error("401");
    content_length = Getfile.tellg();
    Getfile.close();
    PreparHeader();
}


// void    GetMethod::getfromDir()
// {
//     if (first_time)
//     {
//         dir = opendir(path.c_str());
//         if (!dir)
//             throw std::runtime_error("401");
//         if (!(files = readdir(dir)))
//             list_dir = true;
//         if (!indexs.empty() && !files)
//         {
//             for (std::vector<std::string>::iterator it = indexs.begin(); it != indexs.end(); it++)
//             {
//                 if (*it.compare(!))
//             }
//         }
//     }
// }

void    GetMethod::getContentTofile(void)
{
    char    buffer[1024];
    int     x;

    x = read(fdFile, buffer, 1023);
    buffer[x] = '\0';
    if (!x)
        return ;
    if (first_time)
    {
        header.append(buffer, x);
        fdFile = open(path.c_str(), O_RDONLY);
        first_time = false;
        write(fd_socket, header.c_str(), header.length());
    }
    else
    {
        write(fd_socket, buffer, strlen(buffer));
    }
}

GetMethod::~GetMethod()
{
    close(fdFile);
}