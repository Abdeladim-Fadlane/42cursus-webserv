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

int Methods(int fd,Method &object)
{
    std::string line;
    // ssize_t bytesRead;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    read(fd, buffer, sizeof(buffer));
    line == buffer;
    ssize_t pos = line.find("\r\n\r\n") ;
    line = line.substr(0,pos);
    // std::cout << line << std::endl;
    paceUrl(line, object);
    // std::cout << "Method: " << object.addressIp << std::endl;
    // std::cout << "Version: " << object.port << std::endl;
    return 1;
}