/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 16:45:59 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/15 17:51:25 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi(const PostMethod& post, const char *file_dup, const std::string remote_add)
{
    p = post;
    size = 0;
    this->remote_add = remote_add;
    file_script = "/script.php";
    fd_dupin = open(file_dup, O_RDONLY);
}

void  Cgi::init_cgiEnv(void)
{
    std::map<std::string, std::string> m;


    open
    file_script = file_script.substr(file_script.rfind("/"));
    m["REQUEST_METHOD: "] =  "POST";
    m["QUERY_STRING: "] =  p.getPath().substr(p.getPath().find("?") + 1, p.getPath().find("#")); //path.find("?" , "#");
    m["SERVER_PROTOCOL: "] =  "HTTP/1.1";
    m["PATH_INFO: "] =  p.getPath.substr();
    m["SCRIPT_NAME: "] =  "script.php";
    m["CONTENT_TYPE: "] =  p.getContentType();
    m["CONTENT_LENGTH: "] =  p.getContentLength();
    m["SERVER_PORT: "] =  "8080";
    // m["REMOTE_ADDR: "] =  ; // khssni n3amrha 
    // m["REMOTE_ADDR: "] =  "REQUESTE_METHOD";
    env = new char*[m.size() + 1];
    for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++)
        env[size++] = strdup((it->first + it->second).c_str());
    env[size] = NULL;
}

void Cgi::Run_CgiPost()
{
    int pid;
    char **arg;

    arg = new char*[3];
    arg[2] = NULL;
    arg[1] = strdup(file_script.c_str());
    arg[0] = strdup("/usr/bin/php");
    pid = fork();
    if (pid != -1)
    {
        dup2(fd_dupin, 0);
        dup2(fd_dupout, 1);
        close(fd_dupin);
        close(fd_dupout);
        execve(arg[0], arg, env);
        exit(1);
    }  
}

Cgi::~Cgi()
{
    for (int i = 0; i < size; i++)
        delete env[i];
    delete[] env;
}