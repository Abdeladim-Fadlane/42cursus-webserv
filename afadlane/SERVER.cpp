#include"webserv.hpp"

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
            close(socketFD);
            std::cerr<<("setsockopt(SO_REUSEADDR) failed");
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
            throw std::runtime_error("Error listen\n");
        }
        event.events = EPOLLIN ;
        event.data.fd = socketFD;
        if(epoll_ctl(epollFD,EPOLL_CTL_ADD,socketFD,&event) == -1)
        {
            close(socketFD);
            std::cerr<<("Error add to epoll : ");
            continue;
        }
        Servers.push_back(socketFD);
    }
    std::map<int,struct Webserv> Clients;
    epoll_event events[MAX_EVENTS];
    while (true)
    {
        int clientSocketFD;
        int numEvent = epoll_wait(epollFD,events,MAX_EVENTS,-1);
        for (int i = 0; i < numEvent; ++i)
        {
            if(isServer(Servers,events[i].data.fd) == true)
            {
                clientSocketFD = accept(events[i].data.fd, NULL, NULL);
                if(clientSocketFD == -1)
                {
                    std::cerr << "Failed to accept connection ." << std::endl;
                    continue;
                }
                event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLRDHUP ;
                event.data.fd = clientSocketFD;
                if(epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocketFD, &event) == -1)
                {
                    std::cerr<<("error epoll_ctl ");
                    close(clientSocketFD);
                    continue;
                }
                inisialData(Clients,config,clientSocketFD);
            } 
            else
            {
                if(events[i].events & EPOLLRDHUP || events[i].events & EPOLLHUP || events[i].events & EPOLLERR)
                {
                    if(Clients[events[i].data.fd].data.OBJCGI.pid != -1)
                    {
                        int status;
                        std::remove(Clients[events[i].data.fd].data.OBJCGI.cgiFile.c_str());
                        kill(Clients[events[i].data.fd].data.OBJCGI.pid,SIGKILL);
                        waitpid(Clients[events[i].data.fd].data.OBJCGI.pid,&status,0);
                    }
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                    delete Clients[events[i].data.fd].data.requeste;
                    Clients.erase(events[i].data.fd);
                }
                else if(events[i].events & EPOLLIN && Clients[events[i].data.fd].data.isDone == false)
                {
                    if(Clients[events[i].data.fd].data.AlreadyRequestHeader == false)
                    {
                        Clients[events[i].data.fd].data.requeste->readFromSocketFd(Clients[events[i].data.fd].data.isDone, Clients[events[i].data.fd].data.AlreadyRequestHeader);
                        insialStruct(Clients[events[i].data.fd].data);
                    }
                    else if(Clients[events[i].data.fd].data.AlreadyRequestHeader  == true && 
                        Clients[events[i].data.fd].data.requeste->method == "POST")
                    {

                        Clients[events[i].data.fd].data.requeste->post->PostingFileToServer(Clients[events[i].data.fd].data.isDone, true);
                    }
                }
                else if(getCurrentTime() - Clients[events[i].data.fd].data.requeste->time_out > 10 && 
                    Clients[events[i].data.fd].data.requeste->skeeptime_out == false)
                {                   
                    Clients[events[i].data.fd].data.code = 408;
                    Clients[events[i].data.fd].data.statusCode = " 408 Request Timeout";
                    Clients[events[i].data.fd].data.requeste->post->unlink_all_file();
                    sendErrorResponse(Clients[events[i].data.fd].data);
                    if(Clients[events[i].data.fd].data.readyForClose == true)
                    {
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                        delete Clients[events[i].data.fd].data.requeste;
                        Clients.erase(events[i].data.fd);
                    }
                }
                else if (events[i].events & EPOLLOUT && Clients[events[i].data.fd].data.isDone == true)
                {
                    if(Clients[events[i].data.fd].data.code == 0)
                    {
                        if(Clients[events[i].data.fd].data.requeste->method == "GET" )
                            Clients[events[i].data.fd].data.OBJGET.getMethod(Clients[events[i].data.fd].data);
                        else if(Clients[events[i].data.fd].data.requeste->method == "DELETE")
                        {
                            if(Clients[events[i].data.fd].data.isDelete == false)
                                Clients[events[i].data.fd].data.OBJDEL.deleteMethod(Clients[events[i].data.fd].data); 
                        }
                        else if(Clients[events[i].data.fd].data.requeste->method == "POST" )
                        {
                            if(Clients[events[i].data.fd].data.requeste->post->isCgi == true)
                            {
                                std::string type = Clients[events[i].data.fd].data.requeste->post->cgi_extation;
                                Clients[events[i].data.fd].data.OBJCGI.fastCGI(Clients[events[i].data.fd].data,type);
                            }
                            else
                                Clients[events[i].data.fd].data.requeste->set_status_client(Clients[events[i].data.fd].data.readyForClose);
                        }
                        else
                            Clients[events[i].data.fd].data.requeste->set_status_client(Clients[events[i].data.fd].data.readyForClose);
                    }
                    else if(Clients[events[i].data.fd].data.code != 0)
                        sendErrorResponse(Clients[events[i].data.fd].data);
                    if(Clients[events[i].data.fd].data.readyForClose == true)
                    {
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                        delete Clients[events[i].data.fd].data.requeste;
                        Clients.erase(events[i].data.fd);
                    }
                }
            }
        }
    }
    close(epollFD);
    closeServers(Servers);
}
