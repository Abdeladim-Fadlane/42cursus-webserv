/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 16:34:28 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/21 14:29:07 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_METHOD_HPP
#define POST_METHOD_HPP
#include <fstream>
#include <string>
#include <fstream>
#include <string.h>
#include <ctime>
#include <sys/time.h>
#define FILE "/Users/akatfi/Desktop/vovvo/"
#include "Requeste.hpp"

class PostMethod
{
    private:
        static long         i;
        bool                flag_boundary;
        size_t                size; // long
        std::string         boundary_separator;
        size_t              separator_size;
        std::string         buffer_add;
        Requeste            req;
        std::fstream        Postfile;
        std::string         content_type;
        size_t              content_length;
        std::string         Transfer_Encoding;
        bool                first_time;
        struct timeval Time;
        std::map<std::string, std::string>  map_extation;
    public:
        std::string         path;
        PostMethod(const Requeste& r);
        void    PostingFileToServer(int &flag); 
        void    chunked(std::string &buffer);   
        void    boundary(std::string buffer);
        size_t getContentLength(void) const;
        const std::string& getContentType(void) const;
        std::string init_contentType(std::string& buffer);
        ~PostMethod();
};

#endif