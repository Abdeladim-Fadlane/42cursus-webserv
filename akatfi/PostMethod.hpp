/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 16:34:28 by akatfi            #+#    #+#             */
/*   Updated: 2024/02/04 16:06:36 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_METHOD_HPP
#define POST_METHOD_HPP
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <ctime>
#include <sys/time.h>
#include "Requeste.hpp"

class PostMethod
{
    private:
        size_t              size;
        std::string         boundary_separator;
        std::string         buffer_add;
        size_t              buffer_add_size;
        Requeste            &req;
        std::fstream        Postfile;
        std::string         content_type;
        long                content_length;
        std::string         Transfer_Encoding;
        bool                first_time;
        std::stringstream   ss;
        std::fstream        cgi_file;
        struct timeval      Time;
        std::map<std::string, std::string>  map_extation;
        std::vector<std::string>    vector_files;
        long              content_file;
        bool            increment_body;
    public:
        std::string         path;
        std::string         cgi_path;
        std::string         script_path;
        std::string         cgi_extation;
        bool                isCgi;
        PostMethod(Requeste& r);
        void    PostingFileToServer(bool& isdone, bool); 
        void    chunked(std::string buffer, bool& isdone);   
        void    boundary(std::string buffer, bool& isdone);
        size_t getContentLength(void) const;
        const std::string& getContentType(void) const;
        std::string init_contentType(std::string& buffer);
        void    unlink_all_file(void);
        void    ft_prepar_cgi();
        ~PostMethod();
};
size_t hexStringToDecimal(const std::string hexString);

#endif