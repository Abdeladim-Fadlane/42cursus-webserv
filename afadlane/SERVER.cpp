#include"webserv.hpp"

void    inisialData(std::map<int,struct Webserv> &Clients ,ConfigFile &config,int &clientSocketFD)
{
    Webserv  Data;
    Data.data.errorFd               =    -2;
    Data.data.code                  =     0;
    Data.data.Alreadyopen           = false;
    Data.data.Alreadyopen           = false;
    Data.data.isFork                = false;
    Data.data.isExeceted            = false;
    Data.data.isReading             = false;
    Data.data.isReadingCgi          = false;
    Data.data.readyForClose         = false;
    Data.data.Alreadparce           = false;
    Data.data.modeAutoIndex         = false;
    Data.data.sendHeader            = false;
    Data.data.AlreadyRequestHeader  = false;
    Data.data.isDone                = false;
    Data.data.autoIndex             = false;
    Data.data.isDelete              = false;
    Data.data.fd                    = clientSocketFD;
    Data.data.requeste              = new Requeste(clientSocketFD,config);
    Clients[clientSocketFD]         = Data;
}

void    insialStruct(Data & datacleint)
{
    if(datacleint.requeste->Location_Server.autoindex == "ON")
        datacleint.autoIndex = true;
    datacleint.autoFile = datacleint.requeste->Location_Server.indexs;
    datacleint.Path = datacleint.requeste->Location_Server.root;
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
void multiplexing(ConfigFile &config)
{
    std::vector<int> Servers;
    int epollFD = epoll_create(1024);
    epoll_event event;
    int socketFD ;
    for(size_t i = 0 ;i < config.Servers.size(); i++)
    {
        socketFD = socket(AF_INET,SOCK_STREAM,0);
        if (socketFD == -1)
            throw std::runtime_error("Cannot create socket");
        sockaddr_in serverAdress;
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_addr.s_addr = INADDR_ANY;
        serverAdress.sin_port = htons(config.Servers[i].listen);
        int reuse = 1;
        if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        {
            throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
        }
        if(bind(socketFD,(struct sockaddr*)&serverAdress,sizeof(serverAdress)) != 0)
        {
            close(socketFD);
            std::cerr<<"Cannot bind to port : "<<config.Servers[i].listen << std::endl ;
            exit(1);
        }
        if(listen(socketFD,128) == 0)
            std::cout<<"listenning to "<< config.Servers[i].listen <<" [...]" <<std::endl;
        else
        {
            close(socketFD);
            throw std::runtime_error("Error listen\n");
        }
        event.events = EPOLLIN ;
        event.data.fd = socketFD;
        if(epoll_ctl(epollFD,EPOLL_CTL_ADD,socketFD,&event) == -1)
        {
            close(socketFD);
            perror("Error add to epoll : ");
            exit(EXIT_FAILURE);
        }
        Servers.push_back(socketFD);
    }
    std::map<int,struct Webserv> Clients;
    epoll_event events[MAX_EVENTS];
    while (true)
    {
        int clientSocketFD;
        int numEvent = epoll_wait(epollFD,events,MAX_EVENTS,500);
        for (int i = 0; i < numEvent; ++i)
        {
            if(isServer(Servers,events[i].data.fd) == true)
            { 
                clientSocketFD = accept(events[i].data.fd,NULL,NULL);
                if(clientSocketFD == -1)
                {
                    std::cerr << "Failed to accept connection ." << std::endl;
                    continue;
                }
                event.events = EPOLLIN | EPOLLOUT | EPOLLHUP ;
                event.data.fd = clientSocketFD;
                if(epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocketFD, &event) == -1)
                {
                    perror("error epoll_ctl ");
                    close(clientSocketFD);
                    continue;
                }
                inisialData(Clients,config,clientSocketFD);
            } 
            else
            {
                if(events[i].events & EPOLLHUP)
                {
                    delete Clients[events[i].data.fd].data.requeste;
                    Clients.erase(events[i].data.fd);
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }
                else if(events[i].events & EPOLLIN && Clients[events[i].data.fd].data.isDone == false)
                {
                    /* File descriptor ready for writing */
                    if(Clients[events[i].data.fd].data.AlreadyRequestHeader == false)
                    {
                        /* readiing AND parsing Clients */
                        Clients[events[i].data.fd].data.requeste->readFromSocketFd(Clients[events[i].data.fd].data.isDone, Clients[events[i].data.fd].data.AlreadyRequestHeader);
                        insialStruct(Clients[events[i].data.fd].data);
                    }
                    else if(Clients[events[i].data.fd].data.AlreadyRequestHeader  == true && Clients[events[i].data.fd].data.requeste->method == "POST")
                        Clients[events[i].data.fd].data.requeste->post->PostingFileToServer(Clients[events[i].data.fd].data.isDone, true);
                }
                else if (events[i].events & EPOLLOUT && Clients[events[i].data.fd].data.isDone == true)
                {
                   /*  File descriptor ready for reading  */
                    if(Clients[events[i].data.fd].data.requeste->method == "GET")
                    {
                        if(Clients[events[i].data.fd].data.code != 0)
                            sendErrorResponse(Clients[events[i].data.fd].data);
                        else
                             Clients[events[i].data.fd].data.OBJGET.getMethod(Clients[events[i].data.fd].data);
                    }
                    else if(Clients[events[i].data.fd].data.requeste->method == "DELETE")
                    {
                        if(Clients[events[i].data.fd].data.code != 0)
                            sendErrorResponse(Clients[events[i].data.fd].data);
                        else
                        {
                            if(Clients[events[i].data.fd].data.isDelete == false)
                                Clients[events[i].data.fd].data.OBJDEL.deleteMethod(Clients[events[i].data.fd].data);
                        }   
                    }
                    else if(Clients[events[i].data.fd].data.requeste->method == "POST" )
                    {
                        if(Clients[events[i].data.fd].data.requeste->post->isCgi == true)
                        {
                            std::string type = Clients[events[i].data.fd].data.requeste->post->cgi_extation;
                            postCgi(Clients[events[i].data.fd].data,type);
                        }
                        else
                            Clients[events[i].data.fd].data.requeste->set_status_client(Clients[events[i].data.fd].data.readyForClose);
                    }
                    else
                        Clients[events[i].data.fd].data.requeste->set_status_client(Clients[events[i].data.fd].data.readyForClose);
                    if(Clients[events[i].data.fd].data.readyForClose == true)
                    {
                        delete Clients[events[i].data.fd].data.requeste;
                        Clients.erase(events[i].data.fd);
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                    }
                }
            }
        }
    }
    close(epollFD);
    closeServers(Servers);
}
