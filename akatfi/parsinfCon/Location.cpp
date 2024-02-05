/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 18:32:49 by akatfi            #+#    #+#             */
/*   Updated: 2024/02/02 18:27:16 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configfile.hpp"

Location::Location(const std::string&  location_name)
{
    this->location_name = location_name;
    close = true;
    uploadfile = "OFF";
    cgi_allowed = "OFF";
    autoindex = "OFF";
}

Location::Location()
{}

Location& Location::operator=(const Location& obj)
{
    location_name = obj.location_name;
    root = obj.root;
    autoindex = obj.autoindex;
    indexs = obj.indexs;
    allowed_method = obj.allowed_method;
    uploadfile = obj.uploadfile;
    upload_location = obj.upload_location;
    cgi_allowed = obj.cgi_allowed;
    cgi = obj.cgi;
    close = obj.close;
    return (*this);
}

void    Location::setLocationName(const std::string& new_name)
{
    location_name = new_name;
}

void    Location::add_location(std::fstream& os)
{
    std::string input;
    std::vector<std::string>  arg;  
    
    while (getlineFromFile(os, input))
    {
        if (input != "}" && input != "{")
        {
            if (input[input.length() - 1] != ';')
                throw std::runtime_error("Error : the line will has a ;");
            input = input.substr(0, input.length() - 1);
        }
        arg = split_line(input);
        if (!arg[0].compare("{") && close)
            close = false;
        else if (!arg[0].compare("}") && !close)
        {
            close = true;
            break ;
        }
        else if (!arg[0].compare("root") && arg.size() == 2)
        {
            if (!root.empty())
                throw std::runtime_error("Error : duplicate member root in location");
            root = arg[1];
        }
        else if (!arg[0].compare("autoindex") && arg.size() == 2)
        {
            if (arg[1].compare("ON") && arg[1].compare("OFF"))
                throw std::runtime_error("Error : the autoindex will has ON or OFF parameter");
            cgi_allowed = arg[1];
        }
        else if (!arg[0].compare("index"))
        {
            for (std::vector<std::string>::iterator it = arg.begin() + 1; it != arg.end(); it++)
                indexs.push_back(*it);
        }
        else if (!arg[0].compare("allowed_methods"))
        {
            for (unsigned int i = 1; i < arg.size(); i++)
            {
                if (arg[i] != "GET" && arg[i] != "POST" && arg[i] != "DELETE")
                    throw std::runtime_error(std::string("Error : The method ").append(arg[i]) + " is not allowed");
                allowed_method.push_back(arg[i]);
            }
        }
        else if (!arg[0].compare("uploadfile") && arg.size() == 2)
        {
            if (arg[1].compare("ON") && arg[1].compare("OFF"))
                throw std::runtime_error("Error : the uploadfile will has ON or OFF parameter");
            uploadfile = arg[1];
        }
        else if (!arg[0].compare("upload_location") && arg.size() == 2)
            upload_location = arg[1];
         else if (!arg[0].compare("cgi_allowed") && arg.size() == 2)
        {
            if (arg[1].compare("ON") && arg[1].compare("OFF"))
                throw std::runtime_error("Error : the cgi_allowed will has ON or OFF parameter");
            autoindex = arg[1];
        }
        else if (!arg[0].compare("cgi") && arg.size() == 3)
        {
            if (access(arg[1].c_str(), X_OK) == -1)
                throw std::runtime_error(std::string("Error : can't execute file's of ").append(arg[2]) + " extation with this path");
            cgi[arg[2]] = arg[1];
        }
        else
            throw std::runtime_error("Error : the location has member not authorized or not complete");
    }
    if (!close)
        throw std::runtime_error("Error : The brackets of location dosen't closed");
    if (this->root.empty())
        throw std::runtime_error("Error : the location need root and exit");
}

Location::~Location()
{}