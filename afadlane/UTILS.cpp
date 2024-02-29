#include"webserv.hpp"

void sendResponce(Data &dataClient,std::string &data)
{
    if(send(dataClient.fd,data.c_str(),data.size(),0) <= 0)
        throw std::runtime_error("error send");
}

void sendErrorResponse(Data &dataClient)
{
    try
    {
        char buffer[BUFFER_SIZE];
        if(dataClient.errorFd == -2)
        {
            std::string htttpresponce;
            std::string filePath;
            filePath = dataClient.requeste->Server_Requeste.error_pages[dataClient.code];
            struct stat fileInfo;
            stat(filePath.c_str(),&fileInfo);
            std::ostringstream wiss;
            wiss <<fileInfo.st_size;
            htttpresponce = dataClient.requeste->http_v;
            htttpresponce.append(dataClient.statusCode).append("\r\nContent-Type: text/html\r\n");
            htttpresponce.append("Content-Lenght: ").append(wiss.str()).append("\r\n\r\n");
            if(send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0) <=0)
            {
                dataClient.readyForClose = true;
                std::runtime_error("error send");
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
            ssize_t readByte = read(dataClient.errorFd,buffer,BUFFER_SIZE);
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
            if(send(dataClient.fd,htttpresponce.c_str(),htttpresponce.size(),0) <= 0)
            {
                throw std::runtime_error("error send");
                dataClient.readyForClose = true;
            } 
        }
    }
    catch(const std::exception& e)
    {
        dataClient.readyForClose = true;
    } 
}

void    inisialData(std::map<int,Client * > &Clients ,ConfigFile &config,int &clientSocketFD)
{
    Client  *Data                = new Client();
    Data->data.fd                = clientSocketFD;
    Data->data.requeste              = new Requeste(clientSocketFD,config);
    Clients[clientSocketFD]         = Data;
}

void    insialStruct(Data & datacleint)
{
    if(datacleint.requeste->Location_Server.autoindex == "ON")
        datacleint.autoIndex = true;
    datacleint.Path = datacleint.requeste->Location_Server.root;
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

double    getCurrentTime(void)
{
    struct timeval currentTime;
    gettimeofday(&currentTime,NULL);
    return ((currentTime.tv_sec) + (currentTime.tv_usec / 1000000));
}

bool isServer(std::vector<int> & Servers,int index)
{
    for(size_t i  = 0 ; i < Servers.size() ; i++)
    {
        if(Servers[i] == index)
            return true;
    }
    return false;
}

void closeServers(std::vector<int> & Servers)
{
    for(size_t i  = 0 ; i < Servers.size() ; i++)
    {
        close(Servers[i]);
    }
}

void EpollCtrDEL(int epollFD,int fd,std::map<int,Client *>& Clients)
{
    epoll_ctl(epollFD, EPOLL_CTL_DEL,fd, NULL);
    close(fd);
    delete Clients[fd];
    Clients.erase(fd);
}

Data::Data()
{
    errorFd               =    -2;
    code                  =     0;
    isDone                = false;
    isDelete              = false;
    autoIndex             = false;
    Alreadparce           = false;
    readyForClose         = false;
    AlreadyRequestHeader  = false;
}

Data::~Data()
{
    delete requeste;
}