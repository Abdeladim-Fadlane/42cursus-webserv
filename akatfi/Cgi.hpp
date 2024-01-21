/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 16:38:20 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/15 12:32:44 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include "Postmethod.hpp"
#include <fcntl.h>
#include <string.h>

class Cgi
{
    private:
        Postmethod      p;
        int size;
        std::string     file_script;
        char **env;
        int             fd_dupin;
        int             fd_dupout;
        std::string remote_add
    public:
        Cgi(const PostMethod& post);
        void    init_cgiEnv();
        void    Run_CgiPost();
        ~Cgi();
};

#endif 