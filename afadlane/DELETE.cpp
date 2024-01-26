
#include"webserv.hpp"
void    sendResponse(int fd,std::string &version,std::string &status,bool &isReadyForClose)
{
    std::string htmlMessage;
    if(status != " 204 No Content")
         htmlMessage = htmlMessage = "<html><head><center><h1>" + status + "</h1></center></head></html>";
    std::string htttpresponce = version + status + "\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
    send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
    isReadyForClose = true;
}

bool checkPermission(const char *path,int fd,std::string &version,int type,bool &isReadyForClose)
{
    std::string htmlMessage ;
    std::string htttpresponce;
    if(access(path,type) != 0 )
    {
        std::string status = " 403 FORBIDDEN";
        sendResponse(fd,version,status,isReadyForClose);
        return true;
    }
    return false;
}

bool   deleteMethod(int fd ,std::string & reqPath,bool &isReadyForClose)
{
    std::string version = "HTTP/1.1";
    std::string htmlMessage ;
    std::string htttpresponce ;
    const char * path = reqPath.c_str();
   
    if(access(path,F_OK) != 0 )
    {
        std::string status = " 404 NOT FOUND";
        sendResponse(fd,version,status,isReadyForClose);
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
                std::string status = " 409 Conflict";
                sendResponse(fd,version,status,isReadyForClose);
                return true;
            }
            if(unlink(path) == 0)
            {
                htttpresponce = version + " 204 No Content\r\nContent-Type: text/html\r\n\r\n";
                send(fd,htttpresponce.c_str(),htttpresponce.size(),0);
                isReadyForClose = true;
                return true;
            }
        }
        else
        {
            if(path[len -1] != '/')
            {
                std::string status = " 409 Conflict";
                sendResponse(fd,version,status,isReadyForClose);
                return true;
            }
        }
    }
    if(checkPermission(path,fd,version,W_OK,isReadyForClose) == true)
        return true;
    DIR *dir = opendir(path);
    if(!dir)
    {
        std::string status = " 500 Internal Server Error";
        sendResponse(fd,version,status,isReadyForClose);
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
                    deleteMethod(fd,itPath,isReadyForClose);
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
        std::string status = "  204 No Content";
        sendResponse(fd,version,status,isReadyForClose);
    }
    return true;
}
