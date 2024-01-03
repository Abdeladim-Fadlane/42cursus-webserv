#include"webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

struct LocationConfig
{
    std::vector<std::string>index;
    std::string upload;
    std::string path;
    std::string root;
    std::string autoindex;
    std::vector<std::string> methods;
    std::vector<std::string> cgiPath;
    std::string proxyReadTimeout;
};

struct ServerConfig
{
    std::string listen;
    std::string host;
    std::string root;
    std::string clientMaxBodySize;
    std::vector<LocationConfig> locations;
};
void printInfo(std::vector<ServerConfig> &servers)
{
    // std::cout << "Number of servers: " << servers.size() << std::endl;
    for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        std::cout << "Listen: " << it->listen << std::endl;
        std::cout << "Host: " << it->host << std::endl;
        std::cout << "root: " << it->root << std::endl;
        for (std::vector<LocationConfig>::iterator locIt = it->locations.begin(); locIt != it->locations.end(); locIt++)
        {
            std::cout << "PathLocation: " << locIt->path << std::endl;
            std::cout << "autoindex: " << locIt->autoindex << std::endl;
            std::cout << "cgiPath: " << locIt->cgiPath[0] << std::endl;
            std::cout << "cgiPath 2: " << locIt->cgiPath[1] << std::endl;
            std::cout << "root: " << locIt->root << std::endl;
            std::cout << "proxyReadTimeout: " << locIt->proxyReadTimeout << std::endl;
            std::cout << "methods :" << locIt->methods[0] << std::endl;
            std::cout << "methods 2 :" << locIt->methods[1] << std::endl;
            std::cout << "index :" << locIt->index[0] << std::endl;
            std::cout << "index  2 :" << locIt->index[1] << std::endl;
            std::cout << "upload  :" << locIt->upload << std::endl;
            std::cout << "--------------------------------------------------->" << std::endl;
        }
    }
}
void splitArguments(const std::string& arguments, std::vector<std::string>& result) {
    std::istringstream iss(arguments);
    std::string token;
    iss >> token;
    while (iss >> token)
    {
        result.push_back(token);
    }
}

bool parseLocationBlock(LocationConfig &location,std::ifstream &file)
{ 
    std::string line;
    std::string key;
    while(std::getline(file,line))
    {
        std::istringstream wiss(line);
        while (wiss >> key)
        {   
            // std::cout<<"<<<-"<<key<<"->>>\n";
            if (key == "}")
                return true;
            else if (key == "autoindex")
                wiss >> location.autoindex;
            else if (key == "index")
                splitArguments(line,location.index);
            else if (key == "upload")
                wiss >> location.upload;
            else if (key == "path")
                wiss >> location.path ;
            else if (key == "proxy_read_time_out")
                wiss >> location.proxyReadTimeout ;
            else if (key == "root")
                wiss >>  location.root ;
            else if (key == "methods")
                splitArguments(line,location.methods);
            else if (key == "cgi_path")
                splitArguments(line,location.cgiPath);
        }   
    }
    return false;
}

bool parseServerBlock(std::string line, ServerConfig &server,std::ifstream &file)
{
    std::istringstream wiss(line);
    std::string key;
    while (wiss >> key)
    {
        if (key == "}")
            return true;
        else if(key == "server" || key == "{")
            continue;
        else if (key == "listen")
            wiss >> server.listen ;
        else if (key == "host")
            wiss >> server.host;
        else if (key == "root")
            wiss >> server.root ;
        else if (key == "client_max_body_size")
            wiss >> server.clientMaxBodySize;
        else if(key == "location")
        {
            LocationConfig location;
            if(parseLocationBlock(location,file))
            {
                server.locations.push_back(location);
                location = LocationConfig();
            }
            else
                throw std::runtime_error("Error location block...");
        }
        else
            throw std::runtime_error("Error server block ...");
    }
    return false;
}

void parseConfigFile(const std::string &fileName, std::vector<ServerConfig> &servers)
{
    std::ifstream file(fileName.c_str());
    if (!file.is_open()) {
        std::cout << "Error in file\n";
        return;
    }
    std::string line;
    ServerConfig server;

    while (std::getline(file, line))
    {
        if (parseServerBlock(line, server,file) == true)
        {
            servers.push_back(server);
            server = ServerConfig();
        }
    }

    file.close();
}

int main(int ac ,char *av[])
{
    try
    {
       std::vector<ServerConfig> servers;
        const char *file = "webserv.conf";
        if(ac == 2)
            file = av[1];
        parseConfigFile(file ,servers);
        // printInfo(servers);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }
}
