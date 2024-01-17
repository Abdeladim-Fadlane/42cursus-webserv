#include"webserv.hpp"

struct ServerConfig
{
    int port ;
    std::string listen;
    std::string host;
    std::string domainName;
    std::vector<std::string> errorPage;
    std::string clientMaxBodySize;
    int size ;
    // std::vector<LocationConfig> locations;
};
    
void example(std::vector<ServerConfig> &vec)
{
    int i = 0;
    ServerConfig conf;

    while (i < SERVERS)
    {
        conf.listen = "127.0.0.0:8080";
        conf.port = 8080 + i;
        vec.push_back(conf);
        i++;
    }
}
/* Analyze, Douiri */

void multiplexing(Method &method)
{ 
    std::map<std::string,ServerConfig> Servers;
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
        Servers[vec[i].listen] = vec[i]; 
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