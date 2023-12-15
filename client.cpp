#include"webserv.hpp"

int main()
{
    // Step 1: Create a socket for the client
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    // Step 2: Set up server address
    sockaddr_in serverAddress;
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(8080);

    // Step 3: Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error failed to connect to socket" << std::endl;
        close(clientSocket);
        exit(1);
    }
    // Step 4: Send data to the server
    while(true)
    {
        //read data from the server.
        char msg[1024];
        memset(msg,0,1024);
        printf("Input a message to server (Q to quit):");
        scanf("%s",msg);
        if(msg[0] == 'q' || msg[0] == 'Q')
            break;
        write(clientSocket,msg,strlen(msg));
        read(clientSocket,msg,1024);
        printf("recieve message from server : %s\n",msg);
    }
    printf("client is disconnectet\n");

    // Step 6: Close the client socket
    close(clientSocket);

    return 0;
}
