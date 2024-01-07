
#include"webserv.hpp"

#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main() {
    int fd = open("/home/afadlane/webserv/tools/Nio.png", O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Error opening file");
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    ssize_t totalBytesRead = 0;

    while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
        totalBytesRead += bytesRead;
        write(1, buffer, bytesRead);  // Assuming you want to write to standard output (stdout)
    }

    // Output HTTP headers with the correct Content-Length
    std::cout << "HTTP/1.1 200 OK\r\nContent-Length: " << totalBytesRead << "\r\n\r\n";

    // Close the file descriptor
    close(fd);

    return 0;
}

