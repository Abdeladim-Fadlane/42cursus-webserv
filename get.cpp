#include"webserv.hpp"
#include <sys/stat.h>
#include<cstdio>
#include<cstdlib>

void   deleteMethod(const std::string &path,const std::string &version)
{
    struct stat file;
    std::ofstream testfile("tools/index.html");
    if(!testfile.is_open())
    {
        testfile << version+" 404 NOT FOUND\r\n" + "{message: can't opning file}";
        return;
    }
    std::string response;
    if(stat(path.c_str(),&file) == 0)
    {
        if(S_ISREG(file.st_mode))
        {
            if(remove(path.c_str()) == 0)
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
        else if(S_ISDIR(file.st_mode))
        {
            if(rmdir(path.c_str()) == 0)
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
            testfile << version+" 404 NOT FOUND\r\n" + "{message: can't removing file/diroctory}";
            return;
        }
    }
    else
        testfile << version+" 404 NOT FOUND\r\n" + "{message: can't removing file/diroctory}";
    return;
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

int getMethod(int fd)
{
    Method object;
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
    if(object.method == "DELETE")
        deleteMethod(object.path,object.version);
    std::cout << "Method: " << object.method << std::endl;
    
    std::cout << "Version: " << object.version << std::endl;

    return 1;
}
