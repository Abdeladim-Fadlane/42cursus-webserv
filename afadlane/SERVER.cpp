#include"webserv.hpp"

void    inisialBollen(std::map<int,struct Webserv> & Request,int & clientSocketFD)
{
    Webserv                         Data;
    Data.data.Alreadyopen           = false;
    Data.data.isReading             = false;
    Data.data.readyForClose         = false;
    Data.data.Alreadparce           = false;
    Data.data.modeAutoIndex         = false;
    Data.data.isCgi                 = false;
    Data.data.AlreadyRequestHeader  = false;
    Data.data.autoIndex             = true;
    Data.data.fd                    = clientSocketFD;
    Data.data.requeste              = new Requeste(clientSocketFD);
    Request[clientSocketFD]         = Data;
}
/* Analyze, Douiri */
void    insialStruct(Data & datacleint)
{
    if(datacleint.requeste->locationServer.autoindex == "ON")
        datacleint.autoIndex = true;

    datacleint.autoFile = datacleint.requeste->locationServer.indexs;
    datacleint.Path = datacleint.requeste->locationServer.root;
    std::cout<<"path = "<<  datacleint.Path<<std::endl;
}

void multiplexing(ConfigFile &config)
{
 
    int epollFD = epoll_create(1024);
    epoll_event event;
    int socketFD ;

    for(size_t i = 0 ;i < config.Servers.size(); i++)
    {
        socketFD = socket(AF_INET,SOCK_STREAM,0);
        if (socketFD == -1)
        {
            std::cerr << "Error creating socket" << std::endl;
            exit(EXIT_FAILURE);
        }
        sockaddr_in serverAdress;
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_addr.s_addr = INADDR_ANY;
        serverAdress.sin_port = htons(config.Servers[i].listen);
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
            std::cerr<<"Error listen\n";
            exit(EXIT_FAILURE);
        }
        event.events = EPOLLIN ;
        event.data.fd = socketFD;
        if(epoll_ctl(epollFD,EPOLL_CTL_ADD,socketFD,&event) == -1)
        {
            close(socketFD);
            perror("Error add to epoll : ");
            exit(EXIT_FAILURE);
        } 
    }

    std::map<int,struct Webserv> Request;
    epoll_event events[MAX_EVENTS];
    while (true)
    {
        int clientSocketFD;
        int numEvent = epoll_wait(epollFD,events,MAX_EVENTS,500); 
        for (int i = 0; i < numEvent; ++i)
        {
            if(static_cast<int>(events[i].data.fd) <=  static_cast<int>(config.Servers.size() + 3))
            {
                clientSocketFD = accept(events[i].data.fd,NULL,NULL);
                if(clientSocketFD == -1)
                {
                    std::cerr << "Failed to accept connection .2" << std::endl;
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
                inisialBollen(Request,clientSocketFD);
            } 
            else
            {
                if(events[i].events & EPOLLHUP)
                {
                    /* client closed the connection */
                    std::cerr<<"An error aka client disconnect\n";
                    Request.erase(events[i].data.fd);
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }
                else if(events[i].events & EPOLLIN )
                {
                    /* File descriptor ready for writing */
                    if(Request[events[i].data.fd].data.AlreadyRequestHeader == false)
                    {
                        /* readiing AND parsing request */
                        Request[events[i].data.fd].data.requeste->readFromSocketFd(Request[events[i].data.fd].data.AlreadyRequestHeader,events[i].data.fd);
                        insialStruct(Request[events[i].data.fd].data);
                    }
                    else if(Request[events[i].data.fd].data.AlreadyRequestHeader  == true && Request[events[i].data.fd].data.requeste->method == "POST")
                    {
                        /* handle Post method  */
                        Request[events[i].data.fd].data.requeste->post->PostingFileToServer(Request[events[i].data.fd].data.readyForClose);
                    }
                }
                else if (events[i].events & EPOLLOUT && Request[events[i].data.fd].data.AlreadyRequestHeader == true)
                {
                   /*  File descriptor ready for reading  */

                    if(Request[events[i].data.fd].data.requeste->method == "GET")
                    {
                        /* handle Get method  */
                        getMethod(Request[events[i].data.fd].data);
                    }
                    else if(Request[events[i].data.fd].data.requeste->method == "DELETE")
                    {
                        /* handle delete method  */
                        deleteMethod(Request[events[i].data.fd].data);
                    }
                    else if(Request[events[i].data.fd].data.requeste->method == "POST")
                    {
                        /* handle response Post method  */
                        std::string body = "<html><body><h1>Post request successful</h1></body></html>";
                        const std::string httpResponse = "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\n\r\n" + body;
                        send(events[i].data.fd, httpResponse.c_str(), httpResponse.size(), 0);
                        Request[events[i].data.fd].data.readyForClose = true;
                    }
                    if(Request[events[i].data.fd].data.readyForClose == true)
                    {
                        /* close File descriptor of client */
                        Request.erase(events[i].data.fd);
                        delete Request[events[i].data.fd].data.requeste;
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                    }
                }
                else if(events[i].events & EPOLLERR)
                {
                    /* an error has occurred */
                    std::string body = "<html><body><h1>Opss an error occurred. Please try again later.</h1></body></html>";
                    const std::string httpResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n" + body;
                    send(events[i].data.fd, httpResponse.c_str(), httpResponse.size(), 0);
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
