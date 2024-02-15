
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

void deleteMethod(Data &dataClient)
{
    const char * path = dataClient.Path.c_str();
    if (access(path, F_OK) != 0) 
    {
        dataClient.statusCode = " 404 NOT FOUND";
        dataClient.code = 404;
        return;
    }
    struct stat statInfo;
    if (stat(path, &statInfo) != 0)
    {
        dataClient.statusCode = " 500 Internal Server Error";
        dataClient.code = 500;
        return;
    }
    if (S_ISREG(statInfo.st_mode)) 
    { 
        if (unlink(path) == 0) 
        {
            dataClient.statusCode = " 204 No Content";
            dataClient.code = 204;
        } else {
            dataClient.statusCode = " 500 Internal Server Error";
            dataClient.code = 500;
        }
        return;
    } 
    else if (S_ISDIR(statInfo.st_mode)) 
    {
        if (checkPermission(dataClient, path, W_OK) || checkPermission(dataClient, path, X_OK))
            return;
        DIR *dir = opendir(path);
        if (!dir) 
        {
            dataClient.statusCode = " 500 Internal Server Error";
            dataClient.code = 500;
            return;
        }
        struct dirent * it;
        while ((it = readdir(dir)) != NULL) 
        {
            if (strcmp(it->d_name, ".") != 0 && strcmp(it->d_name, "..") != 0)
            {
                std::string itPath = dataClient.Path + it->d_name;
                if (stat(itPath.c_str(), &statInfo) == 0)
                {
                    if (S_ISDIR(statInfo.st_mode))
                    {
                        Data subDirData = dataClient;
                        subDirData.Path = itPath + "/";
                        deleteMethod(subDirData); 
                    } 
                    else
                    {
                        if (unlink(itPath.c_str()) != 0)
                        {
                            dataClient.statusCode = " 500 Internal Server Error";
                            dataClient.code = 500;
                            closedir(dir);
                            return;
                        }
                    }
                } 
                else 
                {
                    dataClient.statusCode = " 500 Internal Server Error";
                    dataClient.code = 500;
                    closedir(dir);
                    return;
                }
            }
        }
        closedir(dir);
        if (rmdir(path) == 0)
        {    
            dataClient.statusCode = " 204 No Content";
            dataClient.code = 204;
        }
    }
}
