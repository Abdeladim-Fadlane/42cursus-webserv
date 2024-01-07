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
void    getMethod(Method &method,int cfd)
{
    int fd = open(method.method.c_str() + 1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if(fd == -1)
    {
        throw std::runtime_error("error file");
    }
    ssize_t bytesRead;
    char  buffer[BUFFER_SIZE];
    write(cfd,"HTTP/1.1 200 OK\r\nContent-Length: 10000 \r\n\r\n",44);

    while(bytesRead= read(fd,buffer,BUFFER_SIZE) > 0)
    {
        write(cfd,buffer,BUFFER_SIZE);
    }
    // fastCGI();
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

int Methods(int fd)
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
    if(object.method == "DELETE")
    {
        /* DELETE METHOD */
        deleteMethod(object.path,object.version);
    }
    else if(object.method == "POST")
    {
        /* POST METHOD */
        return (1);
    }
    else if(object.method == "GET")
    {
        /* GET METHOD */
        getMethod(object,fd);
    }
    else
        return(false);
    std::cout << "Path: " << object.path << std::endl;
    std::cout << "Method: " << object.method << std::endl;
    
    std::cout << "Version: " << object.version << std::endl;

    return 1;
}
