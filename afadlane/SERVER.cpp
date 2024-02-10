#include"webserv.hpp"
void    inisialData(std::map<int,struct Webserv> &Request ,ConfigFile &config,int &clientSocketFD)
{
    Webserv  Data;
    Data.data.errorFd               =    -2;
    Data.data.code                  =     0;
    Data.data.Alreadyopen           = false;
    Data.data.Alreadyopen           = false;
    Data.data.isReading             = false;
    Data.data.isFork                = false;
    Data.data.isExeceted            = false;
    Data.data.isReading             = false;
    Data.data.readyForClose         = false;
    Data.data.Alreadparce           = false;
    Data.data.modeAutoIndex         = false;
    Data.data.isCgi                 = false;
    Data.data.AlreadyRequestHeader  = false;
    Data.data.isDone                = false;
    Data.data.autoIndex             = false;
    Data.data.fd                    = clientSocketFD;
    Data.data.requeste              = new Requeste(clientSocketFD,config);
    Request[clientSocketFD]         = Data;
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

void multiplexing(ConfigFile &config)
{
    std::vector<int> Servers;
    int epollFD = epoll_create(1024);
    epoll_event event;
    int socketFD ;
    int reuse = 1;
    
    for(size_t i = 0 ;i < config.Servers.size(); i++)
    {
        socketFD = socket(AF_INET,SOCK_STREAM,0);
        if (socketFD == -1)
            throw std::runtime_error("Cannot create socket");
        sockaddr_in serverAdress;
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_addr.s_addr = INADDR_ANY;
        serverAdress.sin_port = htons(config.Servers[i].listen);
        if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        {
            throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
            continue;
        }
        if(bind(socketFD,(struct sockaddr*)&serverAdress,sizeof(serverAdress)) != 0)
        {
            close(socketFD);
            std::cerr<<"Cannot bind to port : "<<config.Servers[i].listen << std::endl ;
            continue;
        }
        if(listen(socketFD,128) == 0)
            std::cout<<"listenning to "<< config.Servers[i].listen <<" [...]" <<std::endl;
        else
        {
            close(socketFD);
            std::cout<<"Error listen\n";
            continue;
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
    std::map<int,struct Webserv> Request;
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
                event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP ;
                event.data.fd = clientSocketFD;
                if(epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocketFD, &event) == -1)
                {
                    perror("error epoll_ctl ");
                    close(clientSocketFD);
                    continue;
                }
                inisialData(Request,config,clientSocketFD);
            } 
            else
            {
                if(events[i].events & EPOLLHUP)
                {
                    /* client closed the connection */
                    close(Request[events[i].data.fd].data.fileFd);
                    unlink(Request[events[i].data.fd].data.cgiFile.c_str());
                    //     std::cerr<<("sfgdadfg\n");
                    // std::cerr<<"close ="<<Request[events[i].data.fd].data.fd<<"\n";
                    delete Request[events[i].data.fd].data.requeste;
                    Request.erase(events[i].data.fd);
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }
                else if(events[i].events & EPOLLIN && Request[events[i].data.fd].data.isDone == false)
                {
                    /* File descriptor ready for writing */
                    if(Request[events[i].data.fd].data.AlreadyRequestHeader == false)
                    {
                        /* readiing AND parsing request */
                        Request[events[i].data.fd].data.requeste->readFromSocketFd(Request[events[i].data.fd].data.isDone, Request[events[i].data.fd].data.AlreadyRequestHeader);
                        insialStruct(Request[events[i].data.fd].data);
                    }
                    else if(Request[events[i].data.fd].data.AlreadyRequestHeader  == true && Request[events[i].data.fd].data.requeste->method == "POST")
                    {
                        /* handle Post method  */
                        Request[events[i].data.fd].data.requeste->post->PostingFileToServer(Request[events[i].data.fd].data.isDone);
                    }
                }
                else if (events[i].events & EPOLLOUT && Request[events[i].data.fd].data.isDone == true)
                {
                   /*  File descriptor ready for reading  */
                    if(Request[events[i].data.fd].data.requeste->method == "GET")
                    {
                        /* handle Get method  */
                        // std::cout<<"fd socket = "<<Request[events[i].data.fd].data.fd<<"\n";
                        if(Request[events[i].data.fd].data.code != 0)
                        {
                            // Request[events[i].data.fd].data.readyForClose = true;
                            sendErrorResponse(Request[events[i].data.fd].data);
                        }
                        else
                            getMethod(Request[events[i].data.fd].data);
                    }
                    else if(Request[events[i].data.fd].data.requeste->method == "DELETE")
                    {
                        /* handle delete method  */
                        if(Request[events[i].data.fd].data.code != 0)
                            sendErrorResponse(Request[events[i].data.fd].data);
                        else
                            deleteMethod(Request[events[i].data.fd].data);
                    }
                    else if(Request[events[i].data.fd].data.requeste->method == "POST" )
                        Request[events[i].data.fd].data.requeste->set_status_client(Request[events[i].data.fd].data.readyForClose);
                    else
                        Request[events[i].data.fd].data.requeste->set_status_client(Request[events[i].data.fd].data.readyForClose);
                    if(Request[events[i].data.fd].data.readyForClose == true)
                    {
                        Request.erase(events[i].data.fd);
                        delete Request[events[i].data.fd].data.requeste;
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                    }
                }
                else if(events[i].events & EPOLLERR)
                {
                    /* an error has occurred */
                    std::string msg = " 500 Internal Server Error";
                    sendResponse(Request[events[i].data.fd].data,msg);
                    delete Request[events[i].data.fd].data.requeste;
                    Request.erase(events[i].data.fd);
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }
            }
        }
    }
    close(epollFD);
    close(socketFD);
}

