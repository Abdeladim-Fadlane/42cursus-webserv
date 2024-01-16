#include"webserv.hpp"

std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nMethod, GET!\n";
/* Analyze, Douiri */
void multiplexing(Method &method)
{
    int serverSocketFD ;
    int epollFD = epoll_create(5);
    epoll_event event;
    int socketFD ;
    for(int i = 0 ;i < SERVERS; i++)
    {
        socketFD = socket(AF_INET,SOCK_STREAM,0);
        if (socketFD == -1) {
            std::cerr << "Error creating socket" << std::endl;exit(1);}
        sockaddr_in serverAdress;
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_addr.s_addr = INADDR_ANY;
        serverAdress.sin_port = htons(PORT + i);
        if(bind(socketFD,(struct sockaddr*)&serverAdress,sizeof(serverAdress)) != 0)
            std::cerr<<"Cannot bind to port : "<<PORT + i << "\n" ;
        if(listen(socketFD,10) == 0)
            std::cout<<"listenning to "<< PORT + i <<" [...]" <<std::endl;
        else{std::cerr<<"Error listen\n";exit(1);}
        event.events = EPOLLIN ;
        event.data.fd = socketFD;
        if(epoll_ctl(epollFD,EPOLL_CTL_ADD,socketFD,&event) == -1)
            exit(1);
    }
    std::vector<int> readyFD;
    epoll_event events[MAX_EVENTS];
    while (true)
    {
        int clientSocketFD;
        int numEvent = epoll_wait(epollFD,events,MAX_EVENTS,-1); 
        for (int i = 0; i < numEvent; ++i)
        {
            if( events[i].data.fd <= SERVERS + 3)
            {
                clientSocketFD = accept(events[i].data.fd,NULL,NULL);
                
                // std::cout<<"New connections Id : "<< clientSocketFD<<std::endl;
                if(clientSocketFD == -1)
                {std::cerr << "Failed to accept connection ." << std::endl;break;}
                event.events = EPOLLIN | EPOLLOUT;
                readyFD.push_back(clientSocketFD);
                event.data.fd = clientSocketFD;
                if(epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocketFD, &event) == -1)
                {printf("error epoll_ctl ");continue;}
            } 
            else
            {
                if( EPOLLIN )
                {
                    /* readiing */
                    Methods(events[i].data.fd,method);
                }
                if ( EPOLLOUT)
                {
                    /* writing */
                    getMethod(method,events[i].data.fd);
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }
            }
        }
    }
    close(epollFD);
    close(serverSocketFD);
}

/* حلل يا دويري */