#include"webserv.hpp"


int paceUrl(std::string line,Method &object)
{
    std::string firstLine;
    std::string secondLine;
    std::string host;
    std::istringstream wiss(line);
    std::getline(wiss, firstLine);
    std::istringstream iss(firstLine);
    iss >> object.method >> object.path >> object.version; 
    std::getline(wiss, secondLine);
    std::istringstream ciss(secondLine);
    ciss >> firstLine >> host;
    object.host = host;
    size_t pos = host.find(':');
    object.addressIp = host.substr(0,pos);
    object.port = host.substr(pos + 1);
    return(1);
}

void parceRequest(Data & Data,Method &method,int fd)
{
    Data.Alreadparce = 1;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if(read(fd, buffer, sizeof(buffer)) <= 0)
    {
        Data.readyForClose = 1;
        std::runtime_error("error read");
    }
    std::string line(buffer,BUFFER_SIZE);
    ssize_t pos = line.find("\r\n\r\n") ;
    line = line.substr(0,pos);
    paceUrl(line, method);
    // std::cout << line << std::endl;
    // std::cout << "ip: " << method.addressIp << std::endl;
    // std::cout << "port: " << method.port << std::endl;
    // std::cout << "host: " << method.host << std::endl;
    // std::cout << "Version: " << method.version << std::endl;
    // std::cout << "method: " << method.method << std::endl;
    // std::cout << "path: " << method.path << std::endl;
}