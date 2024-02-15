
#include"webserv.hpp"
bool checkPermission(Data &dataClient, const char *path,int type);

void sendErrorResponse(Data &dataClient)
{
    char buffer[BUFFER_SIZE];
    if(dataClient.errorFd == -2)
    {
        std::string htttpresponce;
        std::string filePath = dataClient.requeste->Server_Requeste.error_pages[dataClient.code];
        struct stat fileInfo;
        stat(filePath.c_str(),&fileInfo);
        std::ostringstream wiss;
        wiss <<fileInfo.st_size;
        htttpresponce = dataClient.requeste->http_v.append(dataClient.statusCode).append("\r\nContent-Type: text/html\r\n");
        htttpresponce.append("Content-Lenght: ").append(wiss.str()).append("\n\r\n\r");
        if(send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0) == -1)
        {
            dataClient.readyForClose = true;
            std::runtime_error("error");
        }
        dataClient.errorFd = open(filePath.c_str(),O_RDONLY);
        if(dataClient.errorFd == -1)
        {
            dataClient.readyForClose = true;
            return;
        }
    }
    else
    {
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
        std::string htttpresponce(buffer,readByte);
        if(send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0) == -1)
        {
            throw std::runtime_error("internal Server Error");
            dataClient.readyForClose = true;
        } 
    }
}

bool checkPermission(Data &dataClient, const char *path,int type)
{
    std::string htmlMessage ;
    std::string htttpresponce;
    if(access(path,type) != 0 )
    {
        dataClient.statusCode = " 403 FORBIDDEN";
        dataClient.code = 403;
        
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
         dataClient.statusCode = " 404 NOT FOUND";
        dataClient.code = 404;
        return true;
    }
    struct stat statPath;
    if (stat(path,&statPath) == 0)
    { 
        if(S_ISREG(statPath.st_mode))
        { 
            if(unlink(path) == 0)
            {
                 dataClient.statusCode = "  204 No Content";
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
         dataClient.statusCode = " 500 Internal Server Error";
        dataClient.code = 500;
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
         dataClient.statusCode = "  204 No Content";
        dataClient.readyForClose = true;
    }
    return true;
}
