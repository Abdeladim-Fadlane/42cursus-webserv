#include"webserv.hpp"

void example(std::vector<ServerConfig> &vec)
{
    int i = 0;
    ServerConfig conf;

    while (i < SERVERS)
    {
        conf.listen = "127.0.0.0:8080";
        conf.port = 8080 + i;
        conf.clientMaxBodySize = "10";
        conf.domainName = "afadlane1337.ma";
        conf.root = "/home/afadlane/webserv/afadlane/tools";
        conf.autoFile = "index.html";
        vec.push_back(conf);
        i++;
    }
}
/* Analyze, Douiri */


void multiplexing(Method &method)
{ 
    std::vector<std::pair<std::string,ServerConfig> > Servers;
    std::vector<ServerConfig> vec;
    example(vec);
   
    int serverSocketFD ;
    int epollFD = epoll_create(5);
    epoll_event event;
    int socketFD ;
    
    /* INSTED OF LOOP ON VECTOR  */
   
    for(size_t i = 0 ;i < vec.size(); i++)
    {
        socketFD = socket(AF_INET,SOCK_STREAM,0);
        if (socketFD == -1) {
            std::cerr << "Error creating socket" << std::endl;exit(1);}
        sockaddr_in serverAdress;
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_addr.s_addr = INADDR_ANY;
        serverAdress.sin_port = htons(vec[i].port);
        if(bind(socketFD,(struct sockaddr*)&serverAdress,sizeof(serverAdress)) != 0)
            std::cerr<<"Cannot bind to port : "<<vec[i].port << "\n" ;
        if(listen(socketFD,10) == 0)
            std::cout<<"listenning to "<< vec[i].port <<" [...]" <<std::endl;
        else{std::cerr<<"Error listen\n";exit(1);}
        event.events = EPOLLIN ;
        event.data.fd = socketFD;
        if(epoll_ctl(epollFD,EPOLL_CTL_ADD,socketFD,&event) == -1)
            exit(1);
        Servers.push_back(std::make_pair(vec[i].listen,vec[i]));
    }


    std::map<int,dataClient> readyFD;
    epoll_event events[MAX_EVENTS];
    while (true)
    {
        int clientSocketFD;
        int numEvent = epoll_wait(epollFD,events,MAX_EVENTS,-1); 
        for (int i = 0; i < numEvent; ++i)
        {
            if(events[i].data.fd <=  SERVERS + 3)
            {
                dataClient data;
                clientSocketFD = accept(events[i].data.fd,NULL,NULL);
                if(clientSocketFD == -1)
                {std::cerr << "Failed to accept connection ." << std::endl;break;}
                event.events = EPOLLIN | EPOLLOUT;
                data.data.Alreadyopen = 0;
                data.data.readyForClose = 0;
                readyFD[clientSocketFD] = data;
                event.data.fd = clientSocketFD;
                if(epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocketFD, &event) == -1)
                {printf("error epoll_ctl ");continue;}
            } 
            else
            {
                if(events[i].data.fd & EPOLLIN)
                {
                    /* readiing AND parsing request and POST METHOUD*/
                    // Methods(events[i].data.fd,method);
                }
                if (events[i].data.fd & EPOLLOUT)
                {
                    /* writing and Get methoud */
                    getMethod(readyFD[events[i].data.fd].data,method,Servers,events[i].data.fd);
                    
                    if(readyFD[events[i].data.fd].data.readyForClose == 1)
                    {
                        readyFD.erase(events[i].data.fd);
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                    }
                }
            }
        }
    }
    close(epollFD);
    close(serverSocketFD);
}

/* حلل يا دويري */