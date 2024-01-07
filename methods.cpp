#include"webserv.hpp"
#include <sys/stat.h>
#include<cstdio>
#include<cstdlib>

void   deleteMethod(const std::string &path1,const std::string &version)
{
    const char* path = path1.c_str();
    struct stat fileStat;
    std::ofstream testfile("tools/index.html");
    if(!testfile.is_open())
    {
        testfile << version+" 404 NOT FOUND\r\n" + "{message: can't opning file}";
        return;
    }
    std::string response;
    if(stat(path,&fileStat) == 0)
    {
        std::cout<<"here\n";
        if(S_ISREG(fileStat.st_mode))
        {
            if(remove(path) == 0)
            {
                testfile <<version+" 200 OK\r\n" + "{message: Resource deleted successfully}";
                return; 
            }
            else
            {
                testfile<< version+" 404 NOT FOUND\r\n" + "{message: can't removing file}";
                return ;
            }   
        }
        else if(S_ISDIR(fileStat.st_mode))
        {
            if(rmdir(path) == 0)
            {

                testfile <<version+" 200 OK\r\n" + "{message: Resource deleted successfully}";
                return ;
            }
            else
            {
                testfile<< version+" 404 NOT FOUND\r\n" + "{message: can't removing directory}";
                return ;
            }
        }
        else
        {
            testfile << version+" 404 NOT FOUND\r\n" + "{not such file or directory}";
            return;
        }
    }
    else
        testfile << version+" 404 NOT FOUND\r\n" + "{message: can't removing file/diroctory}";
    testfile.close();
    return;
}
void sendChunk(int clientSocket, const char* data, ssize_t length)
{
    dprintf(clientSocket, "%zx\r\n", length);
    write(clientSocket, data, length);
    dprintf(clientSocket, "\r\n");
}
void    getMethod(Method &method,int cfd)
{
    // int fd = open("/home/afadlane/webserv/tools/Nio.png", O_RDONLY);
    int fd = open("/home/afadlane/webserv/test.mp4", O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Error opening file");
    }
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    ssize_t totalBytesRead = 0;
    // std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n";
    std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: video/mp4\r\nTransfer-Encoding: chunked\r\n\r\n";
    write(cfd, httpResponse.c_str(), httpResponse.size());  
    while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0)
    {
        sendChunk(cfd, buffer, bytesRead);  
    }
    dprintf(cfd, "0\r\n\r\n");
    close(fd);
}

int paceUrl(std::string line,Method *object)
{
    std::istringstream wiss(line);
    if (!(wiss >> object->method >> object->path >> object->version))
    {
        std::cerr << "Error parsing the request." << std::endl;
        return 0;
    }
    return(1);
}

int Methods(int fd,Method &object)
{
    
    std::string line;
    ssize_t bytesRead;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        line += buffer;
        if (line.find("\r\n\r\n") != std::string::npos) {
            break; 
        }
    }
    std::cout << line << std::endl;
    std::string firstLine;
    std::istringstream wiss(line);
    std::getline(wiss, firstLine);
    paceUrl(firstLine, &object);
    std::cout << "Path: " << object.path << std::endl;
    std::cout << "Method: " << object.method << std::endl;
    std::cout << "Version: " << object.version << std::endl;

    return 1;
}
