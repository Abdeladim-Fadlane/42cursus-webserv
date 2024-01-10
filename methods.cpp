#include"webserv.hpp"
#include <sys/stat.h>
#include<cstdio>
#include<cstdlib>

void   deleteMethod(const std::string &path1,const std::string &version)
{
    struct stat fileStat;
    std::ofstream testfile("tools/index.html");
    if(!testfile.is_open())
    {
        testfile << version+" 404 NOT FOUND\r\n" + "{message: can't opning file}";
        return;
    }
    std::string response;
    std::string dir = "www/wordpress";
    std::string fullPath = dir + "/" + path1;
    const char* path = fullPath.c_str();
    if(stat(path,&fileStat) == 0)
    {
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
    std::stringstream chunkHeader;
    chunkHeader << std::hex << length << "\r\n";
    std::string chunkHeaderStr = chunkHeader.str();

    if (write(clientSocket, chunkHeaderStr.c_str(), chunkHeaderStr.size()) < 0)
    {
        // perror("Error writing chunk header");
        throw std::runtime_error("Error writing chunk header");
    }

    if (length > 0 && write(clientSocket, data, length) < 0)
    {
        // perror("Error writing chunk data");
        throw std::runtime_error("Error writing chunk data");
    }

    if (write(clientSocket, "\r\n", sizeof("\r\n") - 1) < 0)
    {
        // perror("Error writing chunk footer");
        throw std::runtime_error("Error writing chunk footer");
    }
}

std::string getFileTYpe(Method &method)
{
    size_t pos = method.path.find_last_of('.');
    if(pos != std::string::npos)
    {
        std::string extension = method.path.substr(pos+1);
        if (extension == "mp4" || extension == "avi" || extension == "mkv") 
            return "video";
        else if (extension == "jpg" || extension == "jpeg" || extension == "png")
            return "image";
        else
            return "file";
    }
    return"directory";
}

#include <map>
std::string    getContentType(Method &method)
{
    std::cout<<method.path<<"\n";
    std::map<std::string, std::string> contentTypeMap;
    contentTypeMap[".html"] = "text/html";
    contentTypeMap[".htm"] = "text/html";
    contentTypeMap[".txt"] = "text/plain";
    contentTypeMap[".jpg"] = "image/jpeg";
    contentTypeMap[".jpeg"] = "image/jpeg";
    contentTypeMap[".png"] = "image/png";
    contentTypeMap[".gif"] = "image/gif";
    contentTypeMap[".mp4"] = "video/mp4";
    contentTypeMap[".php"] = "text/html";
    size_t pos = method.path.find_last_of(".");
    if(pos != std::string::npos)
    {
        std::string extension = method.path.substr(pos);
        std::map<std::string,std::string>::iterator it = contentTypeMap.find(extension);
        if(it != contentTypeMap.end())
            return it->second;
    }
    return "text/html";
}
void getMethod(Method &method, int cfd)
{
    int fd = open("/home/afadlane/webserv/tools/Nio.png", O_RDONLY);
    if (fd == -1)
        throw std::runtime_error("Error opening file");
    char buffer[BUFFER_SIZE];
    memset(buffer,0,sizeof(buffer));
    ssize_t bytesRead;
    std::string httpResponse = method.version + " 200 OK\r\nContent-Type:" + getContentType(method) +" \r\nTransfer-Encoding: chunked\r\n\r\n";
    write(cfd, httpResponse.c_str(), httpResponse.size());  
    while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0)
    {
        try
        {
            sendChunk(cfd, buffer, bytesRead);  

        } catch (const std::runtime_error& e)
        {
            std::cout<<e.what()<<std::endl;
            close(fd);
            return;
        }
    }
    close(fd);
    write(cfd, "0\r\n\r\n", sizeof("0\r\n\r\n") - 1);
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
    // std::cout << line << std::endl;
    std::string firstLine;
    std::istringstream wiss(line);
    std::getline(wiss, firstLine);
    paceUrl(firstLine, &object);
    // std::cout << "Path: " << object.path << std::endl;
    // std::cout << "Method: " << object.method << std::endl;
    // std::cout << "Version: " << object.version << std::endl;
    return 1;
}
