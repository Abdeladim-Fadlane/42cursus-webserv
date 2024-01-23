
#include"webserv.hpp"

void    internalServerError(bool &ready ,std::string version,int fd)
{
    std::string htmlMessage = "<html><head><center><h1>500 Internal Server Error/h1></center></head></html>";
    std::string  htttpresponce = version + "500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
    send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
    ready = true;
}

void   deleteMethod(int fd ,const char *path,bool &ready)
{
    /* success 204 No Content*/    
    std::string version = "HTTP/1.1";
    std::string htmlMessage ;
    std::string htttpresponce ;
    
    if(access(path,F_OK) != 0)
    {
        htmlMessage = "<html><head><center><h1>404 NOT FOUND/h1></center></head></html>";
        htttpresponce = version + "404 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
        send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
        ready = true;
        return ;
    }
    if(access(path,W_OK) != 0)
    {
        htmlMessage = "<html><head><center><h1>403 FORBIDDEN/h1></center></head></html>";
        htttpresponce = version + "403 Forbidden\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
        send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
        ready = true;
        return ;
    }

    /* <center><h1>409 Conflict</h1></center> */
    /* if path di=ont end with / */
    DIR *dir = opendir(path);
    if(!dir)
    {
        internalServerError(ready,version,fd);
        return ;
    }
    struct dirent * it;
    struct stat statInfo;

    while((it = readdir(dir)) != NULL)
    {
        std::string itPath  = std::string(path) + "/" + it->d_name;
        if(strcmp(it->d_name,".") != 0 && strcmp(it->d_name,"..") != 0)
        {
            if(stat(itPath.c_str(),&statInfo) == -1)
            {
                internalServerError(ready,version,fd);
                return ;
            }
            if(S_ISDIR(statInfo.st_mode))
            {
                deleteMethod(fd,itPath.c_str(),ready);
            }
            else
            {
                if(unlink(itPath.c_str()) == -1)
                {
                    internalServerError(ready,version,fd);
                    return ;
                }
            }
        }
    }
    closedir(dir);
    if (rmdir(path) == -1)
    {
        internalServerError(ready,version,fd);
        return ;
    }
    htttpresponce = version + "204 No Content\r\nContent-Type: text/html\r\n\r\n";
    send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
    ready = true;
}
