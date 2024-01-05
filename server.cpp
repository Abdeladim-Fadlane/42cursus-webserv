#include"webserv.hpp"

std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nMethod, GET!\n";

void multiplexing()
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
                
                std::cout<<"New connections Id : "<< clientSocketFD<<std::endl;
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
                std::vector<int>::iterator it ;
                for(it = readyFD.begin(); it != readyFD.end() ;it++)
                {
                    if(*it == events[i].data.fd)
                        break;
                }
                if(events[i].events & EPOLLIN && getMethod(events[i].data.fd))
                {
                    /*
                        readiing
                    */
                }
                if (events[i].events & EPOLLOUT)
                {
                    std::string httpResponse;
                    fastCGI();
                    std::ifstream file ("tools/index.html");
                    if (!file.is_open())
                        std::cerr << "Error opening file 'index.html'" << std::endl;
                    std::string line;
                    std::string content;
                while (std::getline(file, line))
                {
                    if (line.find("Content-type: text/html; charset=UTF-8") != std::string::npos)
                        continue;
                    content += line + "\n";
                }
                    std::stringstream ss;
                    ss << content.size();
                    
                    httpResponse = "HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() + "\r\n\r\n" + content;
                    // write(1,httpResponse.c_str(),httpResponse.size());
                    ssize_t bytesWritten = write(events[i].data.fd,  httpResponse.c_str(),  httpResponse.size());
                    if(bytesWritten == -1)
                    {
                        perror("write");
                        continue;
                    }
                    std::cout<<"Close Client ID : "<< events[i].data.fd <<std::endl;
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                    // return ;
                }
            }
        }
    }
    close(epollFD);
    close(serverSocketFD);
}