#include"webserv.hpp"

void sendResponce(Data &dataClient,std::string &data)
{
    if(send(dataClient.fd,data.c_str(),data.size(),0) <= 0)
        throw std::runtime_error("error send");
}

void openfileAndSendHeader(Data &dataClient)
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
    dataClient.sendHeader = true;
    sendResponce(dataClient,htttpresponce);
    if(dataClient.code == 204)
    {
        dataClient.readyForClose = true;
        return;
    }
    dataClient.fdFile->open(filePath.c_str());
    if(!dataClient.fdFile->is_open())
        throw std::runtime_error("error");
}

void sendBody(Data &dataClient)
{
    char buffer[BUFFER_SIZE];
    dataClient.fdFile->read(buffer,BUFFER_SIZE);
    if(dataClient.fdFile->bad())
        throw std::runtime_error("error");
    std::streamsize readByte = dataClient.fdFile->gcount();
    if(readByte == 0)
    {
        dataClient.readyForClose = true;
        return;
    }
    std::string htttpresponce(buffer,readByte);
    sendResponce(dataClient,htttpresponce);
}

void sendErrorResponse(Data &dataClient)
{
    try
    {
        if(dataClient.sendHeader == false)
            openfileAndSendHeader(dataClient);
        else
            sendBody(dataClient);
    }
    catch(const std::exception& e)
    {
        dataClient.readyForClose = true;
    } 
}

void    createData(std::map<int,Client * > &Clients ,ConfigFile &config,int &clientSocketFD)
{
    Client  *Data                = new Client(clientSocketFD,config);
    Clients[clientSocketFD]      = Data;
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

Data::Data(int &clientSocketFD,ConfigFile &config)
{
    code                  =     0;
    isDone                = false;
    isDelete              = false;
    autoIndex             = false;
    sendHeader            = false;
    Alreadparce           = false;
    readyForClose         = false;
    AlreadyRequestHeader  = false;
    fd                    = clientSocketFD;
    fdFile                = new std::ifstream();
    requeste              = new Requeste(clientSocketFD,config);
}

Data::~Data()
{
    delete requeste;
    delete fdFile;
}