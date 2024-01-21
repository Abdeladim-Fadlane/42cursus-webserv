/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetMethod.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 15:50:13 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/09 17:08:01 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_METHOD_HPP
#define GET_METHOD_HPP
#include <fstream>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include "Requeste.hpp"
#include "GetMethod.hpp"

class GetMethod
{
    private:
        std::ifstream   Getfile;
        Requeste        r;
        std::string     path;
        int             fd_socket;
        long            content_length;
        std::string     content_type;
        bool            first_time;
        int             fdFile;
        DIR             *dir;
        struct dirent   *files;
        bool            list_dir;
        std::string     header;
        // std::vector<std::string>    indexs;
    public:
        GetMethod(const Requeste& req);
        void    PreparHeader(void);
        void    headerOfFile(void);
        void            getContentTofile(void);
        ~GetMethod();
};

#endif