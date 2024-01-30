
#include"webserv.hpp"
void    sendResponse(Data &dataClient,std::string &status)
{
    std::string htmlMessage;
    if(status != "  204 No Content")
         htmlMessage = htmlMessage = "<html><head><center><h1>" + status + "</h1></center></head></html>";
    std::string htttpresponce = dataClient.requeste->http_v + status + "\r\nContent-Type: text/html\r\n\r\n" + htmlMessage;
    send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0);
    dataClient.readyForClose = true;
}

bool checkPermission(Data &dataClient, const char *path,int type)
{
    std::string htmlMessage ;
    std::string htttpresponce;
    if(access(path,type) != 0 )
    {
        std::string status = " 403 FORBIDDEN";
        sendResponse(dataClient,status);
        return true;
    }
    return false;
}

bool   deleteMethod(Data &dataClient)
{
    std::string version = "HTTP/1.1";
    std::string htmlMessage ;
    std::string htttpresponce ;
    const char * path = dataClient.Path.c_str();
   
    if(access(path,F_OK) != 0 )
    {
        std::string status = " 404 NOT FOUND";
        sendResponse(dataClient,status);
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
                sendResponse(dataClient,status);
                return true;
            }
            if(unlink(path) == 0)
            {
                htttpresponce = version + " 204 No Content\r\nContent-Type: text/html\r\n\r\n";
                send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0);
                dataClient.readyForClose = true;
                return true;
            }
        }
        else
        {
            if(path[len -1] != '/')
            {
                std::string status = " 409 Conflict";
                sendResponse(dataClient,status);
                return true;
            }
        }
    }
    if(checkPermission(dataClient,path,W_OK) == true)
        return true;
    DIR *dir = opendir(path);
    if(!dir)
    {
        std::string status = " 500 Internal Server Error";
        sendResponse(dataClient,status);
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
                    dataClient.Path = itPath;
                    itPath += "/";
                    deleteMethod(dataClient);
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
        sendResponse(dataClient,status);
    }
    return true;
}
