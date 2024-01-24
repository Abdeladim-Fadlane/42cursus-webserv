
#include"webserv.hpp"

bool checkPermission(const char *path,int fd,std::string &version,int type)
{
     std::string htmlMessage ;
    std::string htttpresponce;
    if(access(path,type) != 0 )
    {
        htmlMessage = "<html><head><center><h1>403 FORBIDDEN</h1></center></head></html>";
        htttpresponce = version + " 403 Forbidden\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
        send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
        return true;
    }
    return false;
}

bool   deleteMethod(int fd ,std::string & reqPath,bool &ready)
{
    std::string version = "HTTP/1.1";
    std::string htmlMessage ;
    std::string htttpresponce ;
    const char * path = reqPath.c_str();
    std::cout<<path<<"---\n";
    if(access(path,F_OK) != 0 )
    {
        htmlMessage = "<html><head><center><h1>404 NOT FOUND</h1></center></head></html>";
        htttpresponce = version + " 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
        send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
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
            if(unlink(path) == 0)
            {
                htttpresponce = version + " 204 No Content\r\nContent-Type: text/html\r\n\r\n";
                send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
                ready = true;
                return true;
            }
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
   
    if(checkPermission(path,fd,version,W_OK) == true)
    {
        ready = true;
        return true;
    }
    DIR *dir = opendir(path);
    if(!dir)
    {
        htmlMessage = "<html><head><center><h1>500 Internal Server Error</h1></center></head></html>";
        htttpresponce = version + " 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
        send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
        ready = true;
        return true;
    }
    struct dirent * it;
    struct stat statInfo;
    while((it = readdir(dir)) != NULL)
    {
        std::string itPath  = std::string(path) + it->d_name ;
        if(strcmp(it->d_name,".") != 0 && strcmp(it->d_name,"..") != 0)
        {
            if(stat(itPath.c_str(),&statInfo) == 0)
            {
                if(S_ISDIR(statInfo.st_mode))
                {
                    itPath += "/";
                    deleteMethod(fd,itPath,ready);
                }
                else
                {
                    unlink(itPath.c_str());
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
