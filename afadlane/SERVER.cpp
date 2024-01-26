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
        conf.root = "/home/afadlane/webserv/afadlane";
        conf.autoFile = "index.html";
        vec.push_back(conf);
        i++;
    }
}
/* Analyze, Douiri */
void    insialStruct(Data & datacleint)
{
    datacleint.method.path =  datacleint.requeste->path;
    // method.path =  datacleint.requeste->path;
    // std::cout<<"path = "<<  method.path<<std::endl;
    datacleint.method.version = datacleint.requeste->http_v;
    datacleint.method.host = "127.0.0.1:8080";
    datacleint.method.fullPath = std::string("/home/afadlane/webserv/afadlane") + datacleint.method.path;
}

void multiplexing()
{
    // Method method;
    std::vector<std::pair<std::string,ServerConfig> > Servers;
    std::vector<ServerConfig> vec;
    example(vec);
    int epollFD = epoll_create(1024);
    epoll_event event;
    int socketFD ;

    for(size_t i = 0 ;i < vec.size(); i++)
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
        serverAdress.sin_port = htons(vec[i].port);
        if(bind(socketFD,(struct sockaddr*)&serverAdress,sizeof(serverAdress)) != 0)
        {
            close(socketFD);
            std::cerr<<"Cannot bind to port : "<<vec[i].port << std::endl ;
            continue;
        }
        if(listen(socketFD,128) == 0)
            std::cout<<"listenning to "<< vec[i].port <<" [...]" <<std::endl;
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
        Servers.push_back(std::make_pair(vec[i].listen,vec[i]));
    }

    std::map<int,struct Webserv> Request;
    epoll_event events[MAX_EVENTS];
    while (true)
    {
        int clientSocketFD;
        int numEvent = epoll_wait(epollFD,events,MAX_EVENTS,500); 
        for (int i = 0; i < numEvent; ++i)
        {
            if(static_cast<int>(events[i].data.fd) <=  static_cast<int>(Servers.size() + 3))
            {
                Webserv data;
                clientSocketFD = accept(events[i].data.fd,NULL,NULL);
                if(clientSocketFD == -1)
                {
                    std::cerr << "Failed to accept connection .2" << std::endl;
                    continue;
                }
                event.events = EPOLLIN | EPOLLOUT ;
                data.data.Alreadyopen = false;
                data.data.isReading = false;
                data.data.readyForClose = false;
                data.data.Alreadparce = false;
                data.data.modeAutoIndex = false;
                data.data.isCgi = false;
                data.data.AlreadyRequestHeader  = false;
                data.data.requeste = new Requeste(clientSocketFD);
                Request[clientSocketFD] = data;
                event.data.fd = clientSocketFD;
                if(epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocketFD, &event) == -1)
                {
                    perror("error epoll_ctl ");
                    close(clientSocketFD);
                    continue;
                }
            } 
            else
            {
                if(events[i].events & EPOLLIN )
                {
                    /* readiing AND parsing request and POST METHOUD*/
                    if(Request[events[i].data.fd].data.AlreadyRequestHeader == false)
                    {
                        Request[events[i].data.fd].data.requeste->readFromSocketFd(Request[events[i].data.fd].data.AlreadyRequestHeader,events[i].data.fd);
                        insialStruct(Request[events[i].data.fd].data);

                    }
                    else if(Request[events[i].data.fd].data.AlreadyRequestHeader  == true && Request[events[i].data.fd].data.requeste->method == "POST")
                    {
                        /* POST METHOD  */
                        Request[events[i].data.fd].data.requeste->post->PostingFileToServer(Request[events[i].data.fd].data.readyForClose);
                        if(Request[events[i].data.fd].data.readyForClose == true)
                        {
                            Request.erase(events[i].data.fd);
                            delete Request[events[i].data.fd].data.requeste;
                            epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                            close(events[i].data.fd);
                        }
                    } 
                }
                else if (events[i].events & EPOLLOUT && Request[events[i].data.fd].data.AlreadyRequestHeader == true && Request[events[i].data.fd].data.requeste->method == "GET")
                {
                    /* Get Methoud */
                    getMethod(Request[events[i].data.fd].data,Request[events[i].data.fd].data.method,Servers,events[i].data.fd);
                    if(Request[events[i].data.fd].data.readyForClose == true)
                    {
            
                        Request.erase(events[i].data.fd);
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                    }
                }
                else if(events[i].events & EPOLLOUT && Request[events[i].data.fd].data.AlreadyRequestHeader == true && Request[events[i].data.fd].data.requeste->method == "DELETE")
                {
                    /* Delete Method */
                    std::string msg = std::string("/home/afadlane/webserv") + Request[events[i].data.fd].data.method.path;
                    deleteMethod(events[i].data.fd,msg,Request[events[i].data.fd].data.readyForClose);
                    if(Request[events[i].data.fd].data.readyForClose == true)
                    {
                        Request.erase(events[i].data.fd);
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
                else if(events[i].events & EPOLLHUP)
                {
                    /* client closed the connection */
                    perror("An error aka client disconnect");
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
