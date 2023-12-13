#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include<errno.h>
int main() {
    // Step 1: Create a socket for the client
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        fprintf(stderr, "Error creating socket\n");
        exit(1);
    }
    else 
        printf("succefly \n");
    // Step 2: Set up server address
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(1984);

    // Step 3: Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
    printf( "Error failed to connect to socket\n");
    printf("Error code: %d\n", errno);
    printf("Error message: %s\n", strerror(errno));
    exit(1);
    }
    else
        printf("gooos \n");

    // Step 4: Send data to the server
    const char* message = "Hello, this is the client!";
    send(clientSocket, message, strlen(message), 0);

    // Step 5: Receive data from the server
    char buff[1024] = {};
    recv(clientSocket, buff, sizeof(buff), 0);
    printf("Server Response: %s\n", buff);

    // Step 6: Close the client socket
    close(clientSocket);

    return 0;
}
