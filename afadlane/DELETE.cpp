
#include"webserv.hpp"
bool checkPermission(Data &dataClient, const char *path,int type);
void sendErrorResponse(Data &dataClient)
{
    char buffer[BUFFER_SIZE];
    memset(buffer,0,BUFFER_SIZE);
    if(dataClient.errorFd == -2)
    {
        dataClient.errorFd = open(dataClient.requeste->Server_Requeste.error_pages[dataClient.code].c_str(),O_RDONLY);
        if(dataClient.errorFd == -1)
        {
            dataClient.readyForClose = true;
            return;
        }
    }
    ssize_t readByte = read(dataClient.errorFd,buffer,BUFFER_SIZE - 1);
    if(readByte == 0)
    {
        close(dataClient.errorFd);
        dataClient.readyForClose = true;
        return;
    }
    if(readByte < 0)
    {
        close(dataClient.errorFd);
        dataClient.readyForClose = true;
        return;
    }
    std::string htttpresponce(buffer);
    if(send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0) == -1)
    {
        throw std::runtime_error("senddddddd");
        dataClient.readyForClose = true;
    } 
}

void    sendResponse(Data &dataClient,std::string &status)
{
    std::string htttpresponce = dataClient.requeste->http_v + status + "\r\nContent-Type: text/html\r\n\r\n";
    if(send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0) == -1)
    {
        dataClient.readyForClose = true;
        std::runtime_error("ersdgsdgsdror");
    }
}

bool checkPermission(Data &dataClient, const char *path,int type)
{
    std::string htmlMessage ;
    std::string htttpresponce;
    if(access(path,type) != 0 )
    {
        std::string status = " 403 FORBIDDEN";
        dataClient.code = 403;
        sendResponse(dataClient,status);
        return true;
    }
    return false;
}

bool   deleteMethod(Data &dataClient)
{
    std::string htmlMessage ;
    std::string htttpresponce ;
    const char * path = dataClient.Path.c_str();
   
    if(access(path,F_OK) != 0 )
    {
        std::string status = " 404 NOT FOUND";
        dataClient.code = 404;
        sendResponse(dataClient,status);
        return true;
    }
    struct stat statPath;
    if (stat(path,&statPath) == 0)
    { 
        if(S_ISREG(statPath.st_mode))
        { 
            if(unlink(path) == 0)
            {
                std::string status = "  204 No Content";
                sendResponse(dataClient,status);
                dataClient.readyForClose =true;
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
        dataClient.code = 500;
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
        dataClient.readyForClose = true;
    }
    return true;
}
