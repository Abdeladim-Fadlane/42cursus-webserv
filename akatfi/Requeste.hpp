/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requeste.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:13:07 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/20 12:22:35 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTE_HPP
#define REQUESTE_HPP
#include <iostream>
#include <map>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <fstream>
class PostMethod;
class Requeste
{
    private:
        int                                 fd_socket;
        std::string                         head;
        std::string                         http_v;
        std::string                         path;
        std::string                         body;
        // int                                 port;
        // std::string                         host;
        // std
    public:
        PostMethod              *post;
        std::string                         method;
        bool                                RunMethod;
        std::map<std::string, std::string>  requeste_map;
        Requeste(int fd);
        void    MakeMapOfHeader();
        // void    readFromSocketFd();
        void readFromSocketFd(bool &flag,int fd);
        // void    writeToSocketFd(const std::string& buffer);
        // void    MakeValue();
        std::pair<std::string, std::string> MakePair(std::string& line);
        std::map<std::string, std::string> getRequesteMap();
        int     getSocketFd() const;
        // void setFileextation(const std::string& f);
        const std::string& getBody() const;
        const std::string&    getPath() const;
        ~Requeste();
};

void setFileextation(std::string const& f, std::map<std::string, std::string>& Extation);

#endif