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

bool check_location_name(std::string& name_location)
{
    std::stringstream ss;
    std::string spl;
    
    ss << name_location;
    while (std::getline(ss, spl, '/'))
        if (spl == "..")
            return true;
    return false;
}

Location::Location(std::string&  location)
{
    this->location_name = delete_Or_add_slash(location, true, false);
    if (check_location_name(location))
        throw std::runtime_error("Error : the location " + location  + " is not allowed in config file");
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
    redirection = obj.redirection;
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
        if (input.empty() == true)
            continue;
        if (input != "}" && input != "{")
        {
            if (input[input.length() - 1] != ';')
                throw std::runtime_error("Error : the line will has a ;");
            input = input.substr(0, input.length() - 1);
        }
        arg = split_line(input);
        if (!arg[0].compare("{") &&  close)
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
            if (access(arg[1].c_str(), F_OK) == -1)
                throw std::runtime_error("Error : the root location '" + arg[1] + "' is not exist");
            root = delete_Or_add_slash(arg[1], true, true);
        }
        else if (!arg[0].compare("autoindex") && arg.size() == 2)
        {
            if (arg[1].compare("ON") && arg[1].compare("OFF"))
                throw std::runtime_error("Error : the autoindex will has ON or OFF parameter");
            autoindex = arg[1];
        }
        else if (!arg[0].compare("index") && arg.size() != 1)
        {
            if (arg.size() > 2)
                throw std::runtime_error("Error : the index attribut can store one index no more");
            indexs.push_back(arg[1]);
        }
        else if (!arg[0].compare("allowed_methods") && arg.size() != 1)
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
        {
            if (access(arg[1].c_str(), F_OK) == -1)
                throw std::runtime_error("Error : the upload location '" + arg[1] + "' is not exist");
            upload_location = delete_Or_add_slash(arg[1], true, false);
        }
        else if (!arg[0].compare("redirection") && arg.size() == 2)
            redirection = arg[1];
        else if (!arg[0].compare("cgi_allowed") && arg.size() == 2)
        {
            if (arg[1].compare("ON") && arg[1].compare("OFF"))
                throw std::runtime_error("Error : the cgi_allowed will has ON or OFF parameter");
            cgi_allowed = arg[1];
        }
        else if (!arg[0].compare("cgi") && arg.size() == 3)
        {
            if (access(arg[1].c_str(), X_OK) == -1)
                throw std::runtime_error(std::string("Error : can't execute file's of '").append(arg[2]) + "' extation with this path '" + arg[1] + "'");
            if (arg[2] != ".php" && arg[2] != ".py" && arg[2] != ".pl")
                throw std::runtime_error("Error : the server not support this '" + arg[2] + "' extation in cgi");
            cgi[arg[2]] = arg[1];
        }
        else
            throw std::runtime_error("Error : the location has member not authorized or not complete");
    }
    if (!close)
        throw std::runtime_error("Error : The brackets of location dosen't closed");
    if (this->root.empty())
        throw std::runtime_error("Error : set up root for every location");
}

Location::~Location()
{}