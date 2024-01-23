
#include"webserv.hpp"

void    internalServerError(bool &ready ,std::string version,int fd)
{
    std::string htmlMessage = "<html><head><center><h1>500 Internal Server Error</h1></center></head></html>";
    std::string  htttpresponce = version + " 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
    send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
    ready = true;
}

bool   deleteMethod(int fd ,std::string & reqPath,bool &ready)
{
    /* check file is has no permissions */  
    std::string version = "HTTP/1.1";
    std::string htmlMessage ;
    std::string htttpresponce ;
    if(reqPath == "/")
    {
        htmlMessage = "<html><head><center><h1>403 FORBIDDEN</h1></center></head></html>";
        htttpresponce = version + " 403 Forbidden\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
        send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
        ready = true;
        return ;
    }
    const char * path = reqPath.c_str();
    if(access(path,F_OK) != 0)
    {
        htmlMessage = "<html><head><center><h1>404 NOT FOUND</h1></center></head></html>";
        htttpresponce = version + " 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
        if(send(fd,htttpresponce.c_str(),htttpresponce.size(),0) == -1)
            throw std::runtime_error("error send");
        ready = true;
        return true;
    }

    struct stat statPath;
    if (stat(path,&statPath) == 0)
    { 
        size_t len = std::strlen(path);
        if(S_ISREG(statPath.st_mode))
        { 
            if(path[len -1] == '/')
            {
                htmlMessage = "<html><head><center><h1>409 Conflict</h1></center></head></html>";
                htttpresponce = version + " 409 Conflict\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
                send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
                ready = true;
                return true;
            }
            if(unlink(path) == -1)
            {
                internalServerError(ready,version,fd);
                throw std::runtime_error("error 1");
                return true;
            }
            htttpresponce = version + " 204 No Content\r\nContent-Type: text/html\r\n\r\n";
            send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
            ready = true;
            return true;
        }
        else
        {
            if(path[len -1] != '/')
            {
                htmlMessage = "<html><head><center><h1>409 Conflict</h1></center></head></html>";
                htttpresponce = version + " 409 Conflict\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
                send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
                ready = true;
                return true;
            }
        }
    }

    if(access(path,W_OK) != 0)
    {
        htmlMessage = "<html><head><center><h1>403 FORBIDDEN</h1></center></head></html>";
        htttpresponce = version + " 403 Forbidden\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
        send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
        ready = true;
        return true;
    }

    DIR *dir = opendir(path);
    if(!dir)
    {
        internalServerError(ready,version,fd);
        throw std::runtime_error("error 2");
        return true;
    }
    struct dirent * it;
    struct stat statInfo;

    while((it = readdir(dir)) != NULL)
    {
        std::string itPath  = std::string(path) + it->d_name ;
        if(strcmp(it->d_name,".") != 0 && strcmp(it->d_name,"..") != 0)
        {
            if(stat(itPath.c_str(),&statInfo) == -1)
            {
                internalServerError(ready,version,fd);
                throw std::runtime_error("error 3");
                return true;
            }
            if(S_ISDIR(statInfo.st_mode))
            {
                itPath += "/";
                deleteMethod(fd,itPath,ready);
            }
            else
            {
                if(unlink(itPath.c_str()) == -1)
                {
                    internalServerError(ready,version,fd);
                    throw std::runtime_error("error 4");
                    return true;
                }
            }
        }
    }
    closedir(dir);
    if (rmdir(path) == 0)
    {    
        htttpresponce = version + " 204 No Content\r\nContent-Type: text/html\r\n\r\n";
        send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
        ready = true;
        return true; 
    }
    return true;
}
