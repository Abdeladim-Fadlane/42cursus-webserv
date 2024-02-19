#include"webserv.hpp"

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
        htttpresponce.append("Content-Lenght: ").append(wiss.str()).append("\r\n\r\n");
        if(send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0) == -1)
        {
            dataClient.readyForClose = true;
            std::runtime_error("error");
        }
        if(dataClient.code == 204)
        {
            dataClient.readyForClose = true;
            return;
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

bool checkPermission(Data &dataClient,int type)
{
    if(access(dataClient.Path.c_str(),type) != 0 )
    {
        dataClient.statusCode = " 403 FORBIDDEN";
        dataClient.code = 403;
        return true;
    }
    return false;
}