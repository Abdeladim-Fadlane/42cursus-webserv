#include"webserv.hpp"

int main()
{
    // Step 1: Create a socket
    int serverSocketFD = socket(AF_INET,SOCK_STREAM,0);
    if (serverSocketFD == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }
    // Step 2: sol the socket options.
    int option = true;
    setsockopt(serverSocketFD,SOL_SOCKET,SO_REUSEADDR,(void*)&option,sizeof(option));

    //  monitoring file descriptorsep 3: Bind the socket to an address and port
    sockaddr_in serverAdress;
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_addr.s_addr = INADDR_ANY;
    serverAdress.sin_port = htons(8080);
    if(bind(serverSocketFD,(struct sockaddr*)&serverAdress,sizeof(serverAdress)) == 0)
        ;// std::cout<<"server bind to port 8080\n";
    else{std::cerr<<"Cannot bind to port 8080 line 20\n";exit(1);}

    //Step 4 : Listen to the client connection requests
    if(listen(serverSocketFD,4) == 0)
        std::cout<<"listenning ...\n";
    else{std::cerr<<"Error listen\n";exit(1);}

    //Step 5 set the ID files settings 
    struct timeval timeout;
    fd_set FDs,FDsCopy;

    FD_ZERO(&FDs);
    FD_SET(serverSocketFD,&FDs);
    int fdMax = serverSocketFD;
    int fdNum;
    char buffer[BUFFER_SISE];

    //this for accept function 
    int connectionServerSocketFD;
    struct sockaddr_in connectionClientAdress;
    socklen_t clientAddressLength = sizeof(connectionClientAdress);
    memset(&connectionClientAdress,0,sizeof(connectionClientAdress));

    while(1)
    {
        FDsCopy = FDs;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        // monitoring multiple file descriptors,
        fdNum = select(fdMax + 1,&FDsCopy,0,0,&timeout);
        if(fdNum == -1)
        {
            std::cerr<<"Error select failed \n";
            break;
        }
        else if(fdNum == 0)
            continue;
        else
        {
            int i;
            for(i = 0; i < fdMax+1;i++)
            {
                //there is a change if file id = i
                if(FD_ISSET(i,&FDsCopy))
                {
                    if(i == serverSocketFD)//client send a request 
                    {
                        connectionServerSocketFD = accept(serverSocketFD,(struct sockaddr *)&connectionClientAdress,&clientAddressLength);
                        FD_SET(connectionServerSocketFD,&FDs);
                        if(connectionServerSocketFD == -1)
                        {
                            std::cerr<<"Error accept failed\n";
                            continue;
                        }
                        else if(fdMax < connectionServerSocketFD)
                            fdMax = connectionServerSocketFD;
                        else
                        {

                        }
                        std::cout<<"Client whith ID "<< connectionServerSocketFD<<" is connected \n";
                    }
                    else//client want to Read a message
                    {
                        memset(buffer,0,BUFFER_SISE);
                        int stringlength = read(i,buffer,BUFFER_SISE);
                        printf("%s \n",buffer);
                        //buffer fih request dyal l3amil dyalna so hadxi khasna nsiftoh LCGI .
                        if(stringlength == 0)
                        {
                            FD_CLR(i,&FDs);
                            close(i);
                            std::cout<<"client with ID "<< i<<" is diconnected\n";
                        }
                        else
                        {
                            char msg[1024];
                            memset(msg,0,1024);
                            scanf("%s",msg);
                            write(i,msg,1024);
                        }
                    }
                }
                else
                {

                }
            }
        }
    }
    close(serverSocketFD);
}