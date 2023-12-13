
#include"webserv.hpp"
const char* RESPONSE = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, client!";

int main() {
    // Step 1: Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Step 2: Bind the socket to an address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return -1;
    }

    // Step 3: Listen for incoming connections
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Error listening for connections" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        // Step 4: Accept incoming connections
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            close(serverSocket);
            return -1;
        }

        std::cout << "Connection accepted from " << inet_ntoa(clientAddress.sin_addr) << std::endl;

        // Step 5: Send HTTP response
        send(clientSocket, RESPONSE, strlen(RESPONSE), 0);
        char buff[1024] = {};
        recv(clientSocket, buff, sizeof(buff), 0);
        std::cout << "Server Response from client : " << buff << std::endl;
        // Step 6: Close the client socket
        close(clientSocket);
    }

    // The server will never reach this point in this simple example

    // Step 7: Close the server socket
    close(serverSocket);

    return 0;
}
